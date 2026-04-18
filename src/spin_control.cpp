#include <Arduino.h>

#include "motor_driver.h"
#include "rc_handler.h"
#include "spin_control.h"
#include "melty_config.h"

static const uint16_t ESC_MIN_US = DRIVE_OUTPUT_MIN_US;
static const uint16_t ESC_NEUTRAL_US = 1500;
static const uint16_t ESC_MAX_US = DRIVE_OUTPUT_MAX_US;
static const uint16_t DRIVE_DEADBAND_US = 10;
static const int INPUT_MAX_OFFSET_US = 2000 - ESC_NEUTRAL_US;
static const int INPUT_MIN_OFFSET_US = ESC_NEUTRAL_US - 1000;
static const int DRIVE_MAX_POS_OFFSET_US = ESC_MAX_US - ESC_NEUTRAL_US;
static const int DRIVE_MAX_NEG_OFFSET_US = ESC_NEUTRAL_US - ESC_MIN_US;
static const int DRIVE_SLOW_INPUT_POS_OFFSET_US = DRIVE_SLOW_INPUT_MAX_US - ESC_NEUTRAL_US;
static const int DRIVE_SLOW_INPUT_NEG_OFFSET_US = ESC_NEUTRAL_US - DRIVE_SLOW_INPUT_MIN_US;
static const int DRIVE_SLOW_OUTPUT_POS_OFFSET_US =
    (DRIVE_SLOW_INPUT_POS_OFFSET_US * DRIVE_MAX_POS_OFFSET_US) / INPUT_MAX_OFFSET_US;
static const int DRIVE_SLOW_OUTPUT_NEG_OFFSET_US =
    (DRIVE_SLOW_INPUT_NEG_OFFSET_US * DRIVE_MAX_NEG_OFFSET_US) / INPUT_MIN_OFFSET_US;

static int clamp_pulse_us(int pulse_width_us) {
  if (pulse_width_us < ESC_MIN_US) return ESC_MIN_US;
  if (pulse_width_us > ESC_MAX_US) return ESC_MAX_US;
  return pulse_width_us;
}

static int apply_deadband_to_pulse(uint16_t pulse_width_us) {
  int centered = (int)pulse_width_us - (int)ESC_NEUTRAL_US;
  if (abs(centered) <= (int)DRIVE_DEADBAND_US) {
    return ESC_NEUTRAL_US;
  }
  return clamp_pulse_us((int)pulse_width_us);
}

static int map_drive_pulse(uint16_t pulse_width_us) {
  int centered = apply_deadband_to_pulse(pulse_width_us) - (int)ESC_NEUTRAL_US;
  if (centered == 0) {
    return ESC_NEUTRAL_US;
  }

  bool positive = centered > 0;
  int magnitude = abs(centered);
  int slow_input_offset = positive ? DRIVE_SLOW_INPUT_POS_OFFSET_US : DRIVE_SLOW_INPUT_NEG_OFFSET_US;
  int slow_output_offset = positive ? DRIVE_SLOW_OUTPUT_POS_OFFSET_US : DRIVE_SLOW_OUTPUT_NEG_OFFSET_US;
  int mapped_offset = 0;

  if (magnitude <= slow_input_offset) {
    mapped_offset = (magnitude * slow_output_offset) / slow_input_offset;
  } else {
    int max_offset = positive ? DRIVE_MAX_POS_OFFSET_US : DRIVE_MAX_NEG_OFFSET_US;
    int fast_input_span = INPUT_MAX_OFFSET_US - slow_input_offset;
    int fast_output_span = max_offset - slow_output_offset;
    int fast_input = magnitude - slow_input_offset;
    mapped_offset = slow_output_offset +
                    (fast_input * fast_output_span) / fast_input_span;
  }

  int sign = positive ? 1 : -1;
  return clamp_pulse_us((int)ESC_NEUTRAL_US + (sign * mapped_offset));
}

int get_max_rpm() {
  return 0;
}

void spin_one_rotation() {
  if (!rc_signal_is_healthy()) {
    motors_off();
    return;
  }

  int motor_1_input_us = map_drive_pulse(rc_get_ch3_pulse_us());
  int motor_1_us = clamp_pulse_us((ESC_NEUTRAL_US * 2) - motor_1_input_us);
  int motor_2_input_us = map_drive_pulse(rc_get_ch2_pulse_us());
  int motor_2_us = clamp_pulse_us(motor_2_input_us);

  motor_1_write_us((uint16_t)motor_1_us);
  motor_2_write_us((uint16_t)motor_2_us);
}
