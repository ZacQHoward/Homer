# Homer Move Port Notes

This folder is a trimmed PlatformIO starting point for the melty controller on a Seeed XIAO ESP32C3.

Included:
- `src/main.cpp` - Arduino entry point
- `src/spin_control.*` - melty timing and translation logic
- `src/rc_handler.*` - RC pulse capture
- `src/motor_driver.*` - motor output abstraction
- `src/led_driver.*` - heading LED control
- `src/accel_handler.*` - H3LIS331 accelerometer readout
- `lib/SparkFun_LIS331_Arduino_Library` - local accelerometer library copy

Disabled for the first port:
- EEPROM persistence
- Battery monitoring
- Watchdog

What you still need to decide:
- Exact GPIO pin mapping for RC inputs, motor outputs, LED, and I2C
- Whether you want persistent config storage later
- Whether you want a watchdog or boot-time arming sequence later
