#include <Arduino.h>

#include "melty_config.h"
#include "rc_handler.h"

struct rc_channel_state_t {
  uint8_t pin;
  volatile uint32_t rise_time_us;
  volatile uint32_t pulse_width_us;
  volatile uint32_t last_pulse_us;
};

static rc_channel_state_t channel_1 = {LEFTRIGHT_RC_CHANNEL_PIN, 0, RC_NEUTRAL_US, 0};
static rc_channel_state_t channel_2 = {FORBACK_RC_CHANNEL_PIN, 0, RC_NEUTRAL_US, 0};
static rc_channel_state_t channel_3 = {THROTTLE_RC_CHANNEL_PIN, 0, RC_NEUTRAL_US, 0};
static rc_channel_state_t channel_4 = {ORIENTATION_RC_CHANNEL_PIN, 0, RC_NEUTRAL_US, 0};

static inline void IRAM_ATTR handle_channel_interrupt(volatile rc_channel_state_t &channel) {
  if (digitalRead(channel.pin) == HIGH) {
    channel.rise_time_us = micros();
  } else {
    uint32_t now_us = micros();
    if (now_us > channel.rise_time_us) {
      uint32_t pulse_width_us = now_us - channel.rise_time_us;
      if (pulse_width_us >= MIN_RC_PULSE_LENGTH && pulse_width_us <= MAX_RC_PULSE_LENGTH) {
        channel.pulse_width_us = pulse_width_us;
        channel.last_pulse_us = now_us;
      }
    }
  }
}

void IRAM_ATTR handle_channel_1_interrupt() {
  handle_channel_interrupt(channel_1);
}

void IRAM_ATTR handle_channel_2_interrupt() {
  handle_channel_interrupt(channel_2);
}

void IRAM_ATTR handle_channel_3_interrupt() {
  handle_channel_interrupt(channel_3);
}

void IRAM_ATTR handle_channel_4_interrupt() {
  handle_channel_interrupt(channel_4);
}

static uint16_t read_channel_pulse_us(volatile rc_channel_state_t &channel) {
  uint32_t pulse_width_us = RC_NEUTRAL_US;
  uint32_t last_pulse_us = 0;

  noInterrupts();
  pulse_width_us = channel.pulse_width_us;
  last_pulse_us = channel.last_pulse_us;
  interrupts();

  if (last_pulse_us == 0) {
    return RC_NEUTRAL_US;
  }

  if (micros() - last_pulse_us > RC_TIMEOUT_US) {
    return RC_NEUTRAL_US;
  }

  if (pulse_width_us < MIN_RC_PULSE_LENGTH) return MIN_RC_PULSE_LENGTH;
  if (pulse_width_us > MAX_RC_PULSE_LENGTH) return MAX_RC_PULSE_LENGTH;
  return (uint16_t)pulse_width_us;
}

static bool pulse_is_near(uint16_t pulse_width_us, uint16_t target_us) {
  return pulse_width_us >= (target_us - RC_CONNECTION_TOLERANCE_US) &&
         pulse_width_us <= (target_us + RC_CONNECTION_TOLERANCE_US);
}

static bool channel_has_recent_pulse(volatile rc_channel_state_t &channel) {
  uint32_t last_pulse_us = 0;

  noInterrupts();
  last_pulse_us = channel.last_pulse_us;
  interrupts();

  if (last_pulse_us == 0) {
    return false;
  }

  return (micros() - last_pulse_us) <= RC_TIMEOUT_US;
}

void init_rc() {
  pinMode(channel_1.pin, INPUT);
  pinMode(channel_2.pin, INPUT);
  pinMode(channel_3.pin, INPUT);
  pinMode(channel_4.pin, INPUT);

  attachInterrupt(digitalPinToInterrupt(channel_1.pin), handle_channel_1_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(channel_2.pin), handle_channel_2_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(channel_3.pin), handle_channel_3_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(channel_4.pin), handle_channel_4_interrupt, CHANGE);
}

bool controller_connected() {
  uint16_t ch1 = rc_get_ch1_pulse_us();
  uint16_t ch2 = rc_get_ch2_pulse_us();
  uint16_t ch3 = rc_get_ch3_pulse_us();
  uint16_t ch4 = rc_get_ch4_pulse_us();

  return pulse_is_near(ch3, RC_NEUTRAL_US) &&
         pulse_is_near(ch1, RC_CONNECTION_LOW_US) &&
         pulse_is_near(ch2, RC_CONNECTION_LOW_US) &&
         pulse_is_near(ch4, RC_CONNECTION_LOW_US);
}

bool rc_signal_is_healthy() {
  return channel_has_recent_pulse(channel_1) &&
         channel_has_recent_pulse(channel_2) &&
         channel_has_recent_pulse(channel_3) &&
         channel_has_recent_pulse(channel_4);
}

uint16_t rc_get_channel_pulse_us(uint8_t channel) {
  switch (channel) {
    case 1:
      return read_channel_pulse_us(channel_1);
    case 2:
      return read_channel_pulse_us(channel_2);
    case 3:
      return read_channel_pulse_us(channel_3);
    case 4:
      return read_channel_pulse_us(channel_4);
    default:
      return RC_NEUTRAL_US;
  }
}

uint16_t rc_get_ch1_pulse_us() {
  return rc_get_channel_pulse_us(1);
}

uint16_t rc_get_ch2_pulse_us() {
  return rc_get_channel_pulse_us(2);
}

uint16_t rc_get_ch3_pulse_us() {
  return rc_get_channel_pulse_us(3);
}

uint16_t rc_get_ch4_pulse_us() {
  return rc_get_channel_pulse_us(4);
}

int rc_get_throttle_percent() {
  uint16_t pulse_width_us = rc_get_ch3_pulse_us();
  if (pulse_width_us <= RC_MIN_US) return 0;
  if (pulse_width_us >= RC_MAX_US) return 100;
  return (int)(((pulse_width_us - RC_MIN_US) * 100UL) / (RC_MAX_US - RC_MIN_US));
}

rc_forback rc_get_forback() {
  uint16_t pulse_width_us = rc_get_ch2_pulse_us();
  if (pulse_width_us > RC_NEUTRAL_US) return RC_FORBACK_FORWARD;
  if (pulse_width_us < RC_NEUTRAL_US) return RC_FORBACK_BACKWARD;
  return RC_FORBACK_NEUTRAL;
}

int rc_get_leftright() {
  return (int)rc_get_ch1_pulse_us() - (int)RC_NEUTRAL_US;
}
