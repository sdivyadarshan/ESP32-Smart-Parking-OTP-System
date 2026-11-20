# Wiring and Pin Connections

| Module | Pin | ESP32 / supply | Notes |
|---|---|---|---|
| Entry IR | OUT | GPIO 13 | Active-LOW assumed |
| Entry IR | VCC | Module-rated 3.3 V / 5 V | Confirm output logic level |
| Entry IR | GND | GND | Common ground |
| Exit IR | OUT | GPIO 12 | Active-LOW assumed |
| Exit IR | VCC | Module-rated 3.3 V / 5 V | Confirm output logic level |
| Exit IR | GND | GND | Common ground |
| I²C LCD | SDA | GPIO 21 | Check pull-up voltage |
| I²C LCD | SCL | GPIO 22 | Check pull-up voltage |
| I²C LCD | VCC | Module-rated supply | Common modules use 5 V |
| I²C LCD | GND | GND | Common ground |
| SG90 | Signal (orange/yellow) | GPIO 14 | PWM |
| SG90 | V+ (red) | Regulated 5 V | Prefer external supply |
| SG90 | GND (brown/black) | Supply GND + ESP32 GND | Common ground |

## Logical wiring overview

```text
Entry IR OUT ------ GPIO 13
Exit IR OUT ------- GPIO 12
LCD SDA ----------- GPIO 21
LCD SCL ----------- GPIO 22
Servo signal ------ GPIO 14
                    ESP32
Servo V+ ---------- regulated 5 V supply
Servo GND --------- supply GND ---- ESP32 GND
All module grounds ---------------- common GND
```

## Assembly

1. Disconnect power before wiring.
2. Connect sensor outputs to GPIO 13 and GPIO 12.
3. Connect LCD SDA/SCL to GPIO 21/GPIO 22.
4. Connect servo signal to GPIO 14.
5. Power the servo using a regulated 5 V supply and join grounds.
6. Verify all GPIO voltage levels, I²C pull-ups and module ratings before powering.

If the wiring changes, update the pin constants at the top of `src/SmartParking/SmartParking.ino`. LCD address is currently `0x27`; some modules use another address. Servo angles 0° and 90° may need mechanical calibration.
