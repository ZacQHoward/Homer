#include <Arduino.h>
#include "motor_driver.h"
#include "rc_handler.h"
#include "spin_control.h"

static const uint16_t RC_CENTER_US = 1500;
static const uint16_t RC_DEADBAND_LOW_US = 1490;
static const uint16_t RC_DEADBAND_HIGH_US = 1510;
static const uint16_t RC_MAX_OFFSET_US = 500;
static const uint16_t ESC_NEUTRAL_US = 1500;
static const uint16_t ESC_FORWARD_MAX_US = 2000;
static const uint16_t ESC_REVERSE_MAX_US = 1000;

static unsigned int highest_rpm = 0;

static uint16_t clamp_us(uint16_t pulse_width_us) {
  if (pulse_width_us < ESC_REVERSE_MAX_US) return ESC_REVERSE_MAX_US;
  if (pulse_width_us > ESC_FORWARD_MAX_US) return ESC_FORWARD_MAX_US;
  return pulse_width_us;
}

int get_max_rpm() {
  return highest_rpm;
}

void spin_one_rotation() {
  uint16_t throttle_us = rc_get_ch3_pulse_us();

  if (throttle_us > RC_DEADBAND_LOW_US && throttle_us < RC_DEADBAND_HIGH_US) {
    motors_off();
    return;
  }

  int offset_us = (int)throttle_us - (int)RC_CENTER_US;
  if (offset_us > (int)RC_MAX_OFFSET_US) offset_us = RC_MAX_OFFSET_US;
  if (offset_us < -(int)RC_MAX_OFFSET_US) offset_us = -(int)RC_MAX_OFFSET_US;

  uint16_t magnitude_us = (uint16_t)abs(offset_us);
  if (magnitude_us > highest_rpm) {
    highest_rpm = magnitude_us;
  }

  uint16_t motor_1_us = clamp_us((uint16_t)((int)ESC_NEUTRAL_US - offset_us));
  uint16_t motor_2_us = clamp_us((uint16_t)((int)ESC_NEUTRAL_US - offset_us));

  motor_1_write_us(motor_1_us);
  motor_2_write_us(motor_2_us);
}
