# Design Decisions

## ESP32 controller
Selected because Wi-Fi, GPIO, PWM and enough processing for a small web server are integrated on one low-cost board.

## Existing Wi-Fi network
ESP32 station mode lets the phone and controller join the same router or mobile hotspot. The LCD displays the controller's local IP. The trade-off is dependence on local network reachability and hotspot/client-isolation behavior.

## Browser-based OTP
A page hosted on the ESP32 avoids a dedicated phone application. It is intentionally a local-network prototype and uses plain HTTP.

## Two IR sensors
Separate sensors distinguish arrival and departure events. Low-cost IR modules can be affected by alignment, ambient light and target reflectivity, so physical calibration is necessary.

## One logical parking slot
One slot demonstrates the end-to-end workflow with low hardware and software complexity. Multi-slot allocation is outside this revision.

## Shared servo barrier
One SG90 is used for the tabletop entry/exit demonstration. It represents one physical model barrier, not independent entry and exit gates.

## Same OTP for one parking session
Retaining the entry OTP through exit makes the demonstration workflow easy to follow. It is weaker than a production session credential and should not be used for real access control.

## Separate servo supply
Servo current spikes can destabilize the ESP32. A regulated 5 V supply with common ground is preferred.

## Non-blocking gate timing
Timestamp-based closure allows the controller to continue servicing web requests and reading sensors while the barrier is open.

## Private credentials
Actual Wi-Fi details live in ignored `secrets.h`; only a placeholder template is version controlled.

## Explicit scope boundary
Cloud connectivity, multi-slot occupancy, database logging, TLS, user accounts and production-grade security are not implemented in this prototype.
