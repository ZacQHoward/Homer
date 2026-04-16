#ifndef HOMER_RC_HANDLER_H
#define HOMER_RC_HANDLER_H

#include <Arduino.h>

typedef enum {
  RC_FORBACK_FORWARD = 1,
  RC_FORBACK_NEUTRAL = 0,
  RC_FORBACK_BACKWARD = -1
} rc_forback;

void init_rc();

bool controller_connected();
bool rc_signal_is_healthy();

uint16_t rc_get_channel_pulse_us(uint8_t channel);
uint16_t rc_get_ch1_pulse_us();
uint16_t rc_get_ch2_pulse_us();
uint16_t rc_get_ch3_pulse_us();
uint16_t rc_get_ch4_pulse_us();

int rc_get_throttle_percent();
rc_forback rc_get_forback();
int rc_get_leftright();

#define RC_MIN_US 1000
#define RC_NEUTRAL_US 1500
#define RC_MAX_US 2000
#define RC_TIMEOUT_US 100000
#define RC_CONNECTION_TOLERANCE_US 5
#define RC_CONNECTION_LOW_US 1100

#define MAX_RC_PULSE_LENGTH 2400
#define MIN_RC_PULSE_LENGTH 700
#define NOMINAL_PULSE_RANGE (MAX_RC_PULSE_LENGTH - MIN_RC_PULSE_LENGTH)
#define MAX_MS_BETWEEN_RC_UPDATES 100

#endif
