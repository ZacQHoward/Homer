#pragma once

#include "melty_config.h"

inline float load_accel_mount_radius() {
  return DEFAULT_ACCEL_MOUNT_RADIUS_CM;
}

inline float load_accel_zero_g_offset() {
  return DEFAULT_ACCEL_ZERO_G_OFFSET;
}

inline float load_heading_led_offset() {
  return DEFAULT_LED_OFFSET_PERCENT;
}

inline void save_settings_to_eeprom(float, float, float) {
}
