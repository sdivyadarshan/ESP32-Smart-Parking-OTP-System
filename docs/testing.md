# Testing and Validation

**Initial status: Not physically validated as a complete system.** This is a test plan; only mark tests passed after performing them on the assembled hardware.

## Pre-power checklist

- [ ] Verify GPIO 13 entry and GPIO 12 exit sensor wiring.
- [ ] Verify GPIO 14 servo signal.
- [ ] Verify LCD SDA 21 / SCL 22.
- [ ] Confirm sensor output is safe for 3.3 V ESP32 GPIO.
- [ ] Confirm LCD I²C pull-ups are voltage-safe.
- [ ] Confirm regulated servo supply and common ground.
- [ ] Inspect for shorts and loose wires.

## Firmware and network

- [ ] Install ESP32 board support and required libraries.
- [ ] Create local `secrets.h` from the example.
- [ ] Compile and upload firmware.
- [ ] Confirm Wi-Fi connection and local IP.
- [ ] Open the IP from a phone on the same Wi-Fi network.

## Entry tests

- [ ] Free slot + entry sensor generates one OTP.
- [ ] OTP and IP are legible on LCD.
- [ ] Wrong entry OTP is rejected.
- [ ] Correct entry OTP marks slot occupied and opens/closes servo.
- [ ] Occupied slot rejects a new entry request.
- [ ] Unused entry OTP expires after two minutes.

## Exit tests

- [ ] Exit sensor activates exit verification only while occupied.
- [ ] Wrong exit OTP does not release the slot.
- [ ] Correct session OTP releases slot and clears OTP.
- [ ] Servo opens and closes after exit approval.
- [ ] Exit request timeout leaves slot occupied.

## Reliability

- [ ] Perform at least 10 complete entry/exit cycles.
- [ ] Check false sensor triggers and alignment.
- [ ] Check servo jitter, power dips and ESP32 resets.
- [ ] Check Wi-Fi disconnect/reconnect behavior.
- [ ] Check behavior after ESP32 reset.

## Observation log

| Test | Expected | Actual | Result |
|---|---|---|---|
| Wi-Fi connection | Connect and show IP | Pending | Not tested |
| Entry sensor | Generate one OTP | Pending | Not tested |
| Correct entry OTP | Occupy slot; open gate | Pending | Not tested |
| Wrong entry OTP | Deny entry | Pending | Not tested |
| Full-slot entry | No new OTP | Pending | Not tested |
| Exit sensor | Request session OTP | Pending | Not tested |
| Wrong exit OTP | Keep slot occupied | Pending | Not tested |
| Correct exit OTP | Release slot; clear OTP | Pending | Not tested |

Record measured connection time, OTP display time, web response time, servo response time, supply voltage during movement and false-trigger count. Add only genuine hardware photographs and demo recordings to `media/`.
