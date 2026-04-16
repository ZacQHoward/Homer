#include <Arduino.h>

#include "melty_config.h"
#include "motor_driver.h"

static const uint16_t ESC_MIN_US = 1000;
static const uint16_t ESC_NEUTRAL_US = 1500;
static const uint16_t ESC_MAX_US = 2000;

static const uint32_t ESC_FRAME_US = 20000;          // 20 ms = 50 Hz
static const uint32_t ESC_PWM_FREQUENCY_HZ = 50;
static const uint8_t ESC_PWM_RESOLUTION_BITS = 12;
static const uint32_t ESC_PWM_MAX_DUTY = (1U << ESC_PWM_RESOLUTION_BITS) - 1U;

// Pick 2 LEDC channels
static const uint8_t MOTOR_1_PWM_CHANNEL = 0;
static const uint8_t MOTOR_2_PWM_CHANNEL = 1;

static uint16_t clamp_pulse_us(uint16_t pulse_width_us) {
  if (pulse_width_us < ESC_MIN_US) return ESC_MIN_US;
  if (pulse_width_us > ESC_MAX_US) return ESC_MAX_US;
  return pulse_width_us;
}

static uint32_t pulse_us_to_duty(uint16_t pulse_width_us) {
  pulse_width_us = clamp_pulse_us(pulse_width_us);

  // duty = pulse_width / frame_width * max_duty
  // Example at 12-bit:
  // 1000 us -> about 205
  // 1500 us -> about 307
  // 2000 us -> about 410
  return ((uint32_t)pulse_width_us * ESC_PWM_MAX_DUTY) / ESC_FRAME_US;
}

static void motor_write_us(uint8_t pwm_channel, uint16_t pulse_width_us) {
  uint32_t duty = pulse_us_to_duty(pulse_width_us);
  ledcWrite(pwm_channel, duty);
}

void motor_1_write_us(uint16_t pulse_width_us) {
  motor_write_us(MOTOR_1_PWM_CHANNEL, pulse_width_us);
}

void motor_2_write_us(uint16_t pulse_width_us) {
  motor_write_us(MOTOR_2_PWM_CHANNEL, pulse_width_us);
}

void motor_1_off() {
  motor_1_write_us(ESC_NEUTRAL_US);
}

void motor_2_off() {
  motor_2_write_us(ESC_NEUTRAL_US);
}

void motors_off() {
  motor_1_off();
  motor_2_off();
}

void init_motors() {
  // Configure PWM channels
  ledcSetup(MOTOR_1_PWM_CHANNEL, ESC_PWM_FREQUENCY_HZ, ESC_PWM_RESOLUTION_BITS);
  ledcSetup(MOTOR_2_PWM_CHANNEL, ESC_PWM_FREQUENCY_HZ, ESC_PWM_RESOLUTION_BITS);

  // Attach pins from melty_config.h
  ledcAttachPin(MOTOR_PIN1, MOTOR_1_PWM_CHANNEL);
  ledcAttachPin(MOTOR_PIN2, MOTOR_2_PWM_CHANNEL);

  motors_off();
}