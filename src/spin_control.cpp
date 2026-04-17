#include <Arduino.h>
#include <math.h>

#include "accel_handler.h"
#include "melty_config.h"
#include "motor_driver.h"
#include "rc_handler.h"
#include "spin_control.h"

static const uint16_t ESC_MIN_US = 1000;
static const uint16_t ESC_NEUTRAL_US = 1500;
static const uint16_t ESC_MAX_US = 2000;
static const uint16_t BASE_SPIN_MAX_OFFSET_US = BASE_SPIN_MAX_US - ESC_NEUTRAL_US;

static const uint16_t RC_CENTER_US = 1500;
static const uint16_t RC_MAX_OFFSET_US = 500;
static const uint16_t THROTTLE_DEADBAND_US = 10;
static const uint16_t TRANSLATION_DEADBAND_US = 5;
static const uint16_t TRIM_DEADBAND_US = 10;

static const float MAX_TRANSLATION_ROTATION_INTERVAL_US =
    (1.0f / MIN_TRANSLATION_RPM) * 60.0f * 1000.0f * 1000.0f;
static const float MAX_TRACKING_ROTATION_INTERVAL_US =
    MAX_TRANSLATION_ROTATION_INTERVAL_US * 2.0f;
static const float MIN_EFFECTIVE_RADIUS_CM = 0.2f;
static const float FULL_CYCLE_RADIANS = 6.28318530718f;
static const float TRIM_RATE_CYCLES_PER_SECOND = 0.35f;

struct translation_vector_t {
  float magnitude;
  float phase_fraction;
};

static unsigned int highest_rpm = 0;
static float manual_phase_trim = 0.0f;
static float accel_mount_radius_cm = DEFAULT_ACCEL_MOUNT_RADIUS_CM;
static float accel_zero_g_offset = DEFAULT_ACCEL_ZERO_G_OFFSET;

static uint16_t clamp_esc_us(int pulse_width_us) {
  if (pulse_width_us < ESC_MIN_US) return ESC_MIN_US;
  if (pulse_width_us > ESC_MAX_US) return ESC_MAX_US;
  return (uint16_t)pulse_width_us;
}

static int get_centered_channel_us(uint16_t pulse_width_us) {
  int centered = (int)pulse_width_us - (int)RC_CENTER_US;
  if (centered > (int)RC_MAX_OFFSET_US) return RC_MAX_OFFSET_US;
  if (centered < -(int)RC_MAX_OFFSET_US) return -(int)RC_MAX_OFFSET_US;
  return centered;
}

static int apply_deadband(int value_us, int deadband_us) {
  if (abs(value_us) <= deadband_us) {
    return 0;
  }
  return value_us;
}

static float wrap_phase_fraction(float phase_fraction) {
  while (phase_fraction < 0.0f) {
    phase_fraction += 1.0f;
  }
  while (phase_fraction >= 1.0f) {
    phase_fraction -= 1.0f;
  }
  return phase_fraction;
}

static float get_spin_direction() {
  int throttle_centered = get_centered_channel_us(rc_get_ch3_pulse_us());
  if (throttle_centered > (int)THROTTLE_DEADBAND_US) return -1.0f;
  if (throttle_centered < -((int)THROTTLE_DEADBAND_US)) return 1.0f;
  return 0.0f;
}

static float get_spin_magnitude_fraction() {
  int throttle_centered = get_centered_channel_us(rc_get_ch3_pulse_us());
  throttle_centered = apply_deadband(throttle_centered, THROTTLE_DEADBAND_US);
  if (throttle_centered == 0) {
    return 0.0f;
  }
  return (float)abs(throttle_centered) / (float)RC_MAX_OFFSET_US;
}

static int compute_base_spin_offset_us(float spin_magnitude_fraction) {
  int offset_us = (int)(spin_magnitude_fraction * (float)BASE_SPIN_MAX_OFFSET_US);
  if (offset_us < 0) return 0;
  if (offset_us > (int)BASE_SPIN_MAX_OFFSET_US) return BASE_SPIN_MAX_OFFSET_US;
  return offset_us;
}

static translation_vector_t compute_translation_vector() {
  int centered_x = apply_deadband(get_centered_channel_us(rc_get_ch1_pulse_us()), TRANSLATION_DEADBAND_US);
  int centered_y = apply_deadband(get_centered_channel_us(rc_get_ch2_pulse_us()), TRANSLATION_DEADBAND_US);

  translation_vector_t result = {};
  if (centered_x == 0 && centered_y == 0) {
    return result;
  }

  float normalized_x = (float)centered_x / (float)RC_MAX_OFFSET_US;
  float normalized_y = (float)centered_y / (float)RC_MAX_OFFSET_US;

  result.magnitude = sqrtf((normalized_x * normalized_x) + (normalized_y * normalized_y));
  if (result.magnitude > 1.0f) {
    result.magnitude = 1.0f;
  }

  // Up = 0.0, right = 0.25, down = 0.5, left = 0.75
  float angle = atan2f(normalized_x, normalized_y);
  result.phase_fraction = wrap_phase_fraction(angle / FULL_CYCLE_RADIANS);
  return result;
}

static float compute_rotation_interval_us() {
  float effective_radius_cm = accel_mount_radius_cm;
  if (effective_radius_cm < MIN_EFFECTIVE_RADIUS_CM) {
    effective_radius_cm = MIN_EFFECTIVE_RADIUS_CM;
  }

  float rpm = fabsf(get_accel_force_g() - accel_zero_g_offset) * 89445.0f;
  rpm = rpm / effective_radius_cm;
  rpm = sqrtf(rpm);

  if (rpm > highest_rpm || highest_rpm == 0) {
    highest_rpm = (unsigned int)rpm;
  }

  if (rpm < 1.0f) {
    rpm = 1.0f;
  }

  float rotation_interval_us = (1.0f / rpm) * 60.0f * 1000.0f * 1000.0f;
  if (rotation_interval_us > MAX_TRACKING_ROTATION_INTERVAL_US) {
    rotation_interval_us = MAX_TRACKING_ROTATION_INTERVAL_US;
  }
  return rotation_interval_us;
}

static void update_manual_phase_trim(float rotation_interval_us) {
  int trim_centered = apply_deadband(get_centered_channel_us(rc_get_ch4_pulse_us()), TRIM_DEADBAND_US);
  if (trim_centered == 0) {
    return;
  }

  float trim_input = (float)trim_centered / (float)RC_MAX_OFFSET_US;
  float rotation_seconds = rotation_interval_us / 1000000.0f;
  manual_phase_trim += trim_input * TRIM_RATE_CYCLES_PER_SECOND * rotation_seconds;
  manual_phase_trim = wrap_phase_fraction(manual_phase_trim);
}

static float compute_final_phase_fraction(const translation_vector_t &translation_vector) {
  return wrap_phase_fraction(translation_vector.phase_fraction + manual_phase_trim);
}

static float phase_delta(float phase_a, float phase_b) {
  float delta = phase_a - phase_b;
  if (delta > 0.5f) delta -= 1.0f;
  if (delta < -0.5f) delta += 1.0f;
  return delta;
}

static bool is_in_translation_half_cycle(float current_phase_fraction,
                                         float start_phase_fraction) {
  float delta = current_phase_fraction - start_phase_fraction;
  if (delta < 0.0f) {
    delta += 1.0f;
  }
  return delta < 0.5f;
}

static int compute_available_translation_us(int signed_base_offset_us) {
  int positive_headroom = ESC_MAX_US - (ESC_NEUTRAL_US + signed_base_offset_us);
  int negative_headroom = (ESC_NEUTRAL_US + signed_base_offset_us) - ESC_MIN_US;
  int available = positive_headroom < negative_headroom ? positive_headroom : negative_headroom;
  if (available < 0) {
    return 0;
  }
  return available;
}

static void handle_translation(float rotation_interval_us,
                               float spin_direction,
                               int base_spin_offset_us,
                               float translation_magnitude,
                               float final_phase_fraction) {
  unsigned long rotation_us = (unsigned long)rotation_interval_us;
  unsigned long start_time_us = micros();
  unsigned long elapsed_us = 0;

  int signed_base_offset_us = (spin_direction >= 0.0f) ? base_spin_offset_us : -base_spin_offset_us;
  int translation_bias_us = (int)(translation_magnitude * (float)compute_available_translation_us(signed_base_offset_us));

  while (elapsed_us < rotation_us) {
    float current_phase_fraction = (rotation_us == 0) ? 0.0f : ((float)elapsed_us / (float)rotation_us);
    bool first_half_cycle = is_in_translation_half_cycle(current_phase_fraction, final_phase_fraction);

    int motor_1_offset_us = signed_base_offset_us;
    int motor_2_offset_us = signed_base_offset_us;

    if (translation_bias_us > 0) {
      if (first_half_cycle) {
        motor_1_offset_us -= translation_bias_us;
        motor_2_offset_us += translation_bias_us;
      } else {
        motor_1_offset_us += translation_bias_us;
        motor_2_offset_us -= translation_bias_us;
      }
    }

    motor_1_write_us(clamp_esc_us(ESC_NEUTRAL_US + motor_1_offset_us));
    motor_2_write_us(clamp_esc_us(ESC_NEUTRAL_US + motor_2_offset_us));

    elapsed_us = micros() - start_time_us;
  }
}

int get_max_rpm() {
  return highest_rpm;
}

void spin_one_rotation() {
  if (!rc_signal_is_healthy()) {
    motors_off();
    return;
  }

  float spin_direction = get_spin_direction();
  float spin_magnitude = get_spin_magnitude_fraction();
  if (spin_direction == 0.0f || spin_magnitude <= 0.0f) {
    motors_off();
    return;
  }
  int base_spin_offset_us = compute_base_spin_offset_us(spin_magnitude);

  translation_vector_t translation_vector = compute_translation_vector();
  float rotation_interval_us = compute_rotation_interval_us();

  update_manual_phase_trim(rotation_interval_us);
  float final_phase_fraction = compute_final_phase_fraction(translation_vector);

  handle_translation(rotation_interval_us,
                     spin_direction,
                     base_spin_offset_us,
                     translation_vector.magnitude,
                     final_phase_fraction);
}
