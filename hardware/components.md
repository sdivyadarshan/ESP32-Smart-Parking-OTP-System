# Hardware Components

| Component | Qty | Purpose |
|---|---:|---|
| ESP32 development board (CP2102 USB-UART) | 1 | Main controller, Wi-Fi and HTTP server |
| SG90 servo | 1 | Shared model barrier |
| 16×2 LCD with I²C backpack | 1 | OTP, local IP and status |
| IR obstacle sensor | 2 | Entry and exit detection |
| Breadboard and jumper wires | As needed | Prototyping |
| Regulated 5 V supply | 1 recommended | Servo power |

## Electrical safety

- ESP32 GPIO uses 3.3 V logic. Never apply 5 V directly to a GPIO.
- Some IR modules powered at 5 V output a 5 V logic HIGH. Verify the module. Use 3.3 V supply only if supported, otherwise use a level shifter or divider.
- Many I²C LCD backpacks pull SDA/SCL up to their supply voltage. If powered at 5 V, check the pull-ups and use a suitable bidirectional I²C level shifter or 3.3 V-safe pull-ups.
- Power the servo from a regulated 5 V source rather than the ESP32 3V3 rail. Connect external supply GND to ESP32 GND.
- Verify the exact module specifications and wiring before powering the circuit.
