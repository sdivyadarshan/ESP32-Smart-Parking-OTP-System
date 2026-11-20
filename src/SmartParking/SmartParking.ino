/*
  ESP32 Smart Parking Access System - single-slot demonstration
  Entry/exit IR sensors: active LOW assumed
  Entry IR: GPIO 13 | Exit IR: GPIO 12
  Servo: GPIO 14 | I2C LCD: SDA 21, SCL 22
  Copy secrets.example.h to secrets.h and configure Wi-Fi privately.
  Physical hardware validation is required.
*/
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

constexpr uint8_t ENTRY_PIN = 13, EXIT_PIN = 12, SERVO_PIN = 14;
constexpr uint8_t LCD_SDA = 21, LCD_SCL = 22;
constexpr int SERVO_CLOSED = 0, SERVO_OPEN = 90;
constexpr unsigned long DEBOUNCE_MS = 40;
constexpr unsigned long OTP_TIMEOUT_MS = 120000UL;
constexpr unsigned long GATE_OPEN_MS = 2000UL;

LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer server(80);
Servo gate;

bool slotOccupied = false, entryPending = false, exitPending = false;
String sessionOtp;
unsigned long requestStarted = 0;
bool gateOpen = false;
unsigned long gateCloseAt = 0;

struct DebouncedInput {
  uint8_t pin;
  bool raw;
  bool stable;
  unsigned long changedAt;
};

DebouncedInput entrySensor{ENTRY_PIN, HIGH, HIGH, 0};
DebouncedInput exitSensor{EXIT_PIN, HIGH, HIGH, 0};

void lcdMessage(const String &line1, const String &line2) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(line1.substring(0, 16));
  lcd.setCursor(0, 1); lcd.print(line2.substring(0, 16));
}

void showCurrentState() {
  if (entryPending) {
    lcdMessage("ENTRY OTP:" + sessionOtp, WiFi.localIP().toString());
  } else if (exitPending) {
    lcdMessage("EXIT: ENTER OTP", "Use same session");
  } else if (slotOccupied) {
    lcdMessage("SLOT OCCUPIED", "Waiting for exit");
  } else {
    lcdMessage("SLOT AVAILABLE", "Waiting vehicle");
  }
}

String makeOtp() {
  char value[5];
  snprintf(value, sizeof(value), "%04u", static_cast<unsigned>(random(1000, 10000)));
  return String(value);
}

bool fallingEdge(DebouncedInput &sensor) {
  bool reading = digitalRead(sensor.pin);
  if (reading != sensor.raw) {
    sensor.raw = reading;
    sensor.changedAt = millis();
  }
  if (sensor.raw != sensor.stable &&
      millis() - sensor.changedAt >= DEBOUNCE_MS) {
    sensor.stable = sensor.raw;
    return sensor.stable == LOW;
  }
  return false;
}

void openGate() {
  gate.write(SERVO_OPEN);
  gateOpen = true;
  gateCloseAt = millis() + GATE_OPEN_MS;
}

void serviceGate() {
  if (gateOpen && static_cast<int32_t>(millis() - gateCloseAt) >= 0) {
    gate.write(SERVO_CLOSED);
    gateOpen = false;
    showCurrentState();
  }
}

String page(const String &notice = "") {
  String form;
  if (entryPending) {
    form = "<h2>Entry verification</h2><p>Enter the OTP shown on the controller LCD.</p>"
      "<form method='POST' action='/verify'><input type='hidden' name='mode' value='entry'>"
      "<input name='otp' inputmode='numeric' pattern='[0-9]{4}' minlength='4' maxlength='4' required>"
      "<button>Verify entry</button></form>";
  } else if (exitPending) {
    form = "<h2>Exit verification</h2><p>Enter the same OTP issued for this session.</p>"
      "<form method='POST' action='/verify'><input type='hidden' name='mode' value='exit'>"
      "<input name='otp' inputmode='numeric' pattern='[0-9]{4}' minlength='4' maxlength='4' required>"
      "<button>Verify exit</button></form>";
  } else if (slotOccupied) {
    form = "<h2>Slot occupied</h2><p>Waiting for exit sensor detection.</p>";
  } else {
    form = "<h2>System ready</h2><p>Waiting for a vehicle at the entry sensor.</p>";
  }

  String html = "<!doctype html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<meta http-equiv='refresh' content='8'><title>ESP32 Smart Parking</title>"
    "<style>body{font:16px Arial;background:#f2f5f9;color:#172033;padding:24px}"
    "main{max-width:440px;margin:7vh auto;background:white;padding:24px;border-radius:14px}"
    "input,button{box-sizing:border-box;width:100%;padding:13px;margin-top:12px;font-size:16px}"
    "button{background:#155eef;color:white;border:0;border-radius:8px}"
    ".notice{background:#eef4ff;padding:10px;border-radius:8px}</style></head><body><main>"
    "<h1>ESP32 Smart Parking</h1>";
  if (notice.length()) html += "<p class='notice'>" + notice + "</p>";
  html += form + "<small>Local-network demonstration prototype</small></main></body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html", page()); }

void handleVerify() {
  if (!server.hasArg("otp") || !server.hasArg("mode") || sessionOtp.isEmpty()) {
    server.send(400, "text/html", page("No active OTP request."));
    return;
  }
  String submitted = server.arg("otp");
  String mode = server.arg("mode");
  if (submitted.length() != 4 || submitted != sessionOtp) {
    server.send(200, "text/html", page("Incorrect OTP. Try again."));
    return;
  }

  if (mode == "entry" && entryPending && !slotOccupied) {
    entryPending = false;
    slotOccupied = true;
    openGate();
    lcdMessage("ENTRY APPROVED", "Gate opening");
    Serial.println("Entry authenticated; slot occupied.");
    server.send(200, "text/html", page("Entry approved. Barrier opening."));
    return;
  }

  if (mode == "exit" && exitPending && slotOccupied) {
    exitPending = false;
    slotOccupied = false;
    sessionOtp = "";
    openGate();
    lcdMessage("EXIT APPROVED", "Slot released");
    Serial.println("Exit authenticated; slot released and OTP cleared.");
    server.send(200, "text/html", page("Exit approved. Slot released."));
    return;
  }
  server.send(409, "text/html", page("This request is not active. Refresh the page."));
}

void setup() {
  Serial.begin(115200);
  pinMode(ENTRY_PIN, INPUT_PULLUP);
  pinMode(EXIT_PIN, INPUT_PULLUP);
  entrySensor.raw = entrySensor.stable = digitalRead(ENTRY_PIN);
  exitSensor.raw = exitSensor.stable = digitalRead(EXIT_PIN);
  entrySensor.changedAt = exitSensor.changedAt = millis();

  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init(); lcd.backlight();
  lcdMessage("SMART PARKING", "Starting...");

  gate.setPeriodHertz(50);
  gate.attach(SERVO_PIN, 500, 2400);
  gate.write(SERVO_CLOSED);
  randomSeed(esp_random());

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcdMessage("Connecting WiFi", "Please wait");
  unsigned long started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000UL) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/verify", HTTP_POST, handleVerify);
    server.begin();
    Serial.print("\nWeb server IP: ");
    Serial.println(WiFi.localIP());
    lcdMessage("WiFi connected", WiFi.localIP().toString());
  } else {
    Serial.println("\nWi-Fi connection failed; restart after checking credentials.");
    lcdMessage("WiFi FAILED", "Check secrets.h");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) server.handleClient();

  if (fallingEdge(entrySensor)) {
    if (!slotOccupied && !entryPending && !exitPending) {
      sessionOtp = makeOtp();
      entryPending = true;
      requestStarted = millis();
      Serial.println("Entry OTP request created.");
      showCurrentState();
    } else if (slotOccupied) {
      lcdMessage("NO SLOTS", "AVAILABLE");
    }
  }

  if (fallingEdge(exitSensor) && slotOccupied && !exitPending && !entryPending) {
    exitPending = true;
    requestStarted = millis();
    Serial.println("Exit verification requested.");
    showCurrentState();
  }

  if (entryPending && millis() - requestStarted >= OTP_TIMEOUT_MS) {
    entryPending = false;
    sessionOtp = "";
    Serial.println("Entry OTP expired.");
    lcdMessage("OTP EXPIRED", "Retry at entry");
  }

  if (exitPending && millis() - requestStarted >= OTP_TIMEOUT_MS) {
    exitPending = false;
    Serial.println("Exit request expired; slot remains occupied.");
    lcdMessage("EXIT TIMEOUT", "Slot occupied");
  }

  serviceGate();
}
