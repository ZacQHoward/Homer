#ifndef HOMER_RC_HANDLER_H
#define HOMER_RC_HANDLER_H

#include <Arduino.h>

void init_rc();

bool rc_signal_is_healthy();

uint16_t rc_get_channel_pulse_us(uint8_t channel);
uint16_t rc_get_ch1_pulse_us();
uint16_t rc_get_ch2_pulse_us();
uint16_t rc_get_ch3_pulse_us();
uint16_t rc_get_ch4_pulse_us();

#define RC_MIN_US 1000
#define RC_NEUTRAL_US 1500
#define RC_MAX_US 2000
#define RC_TIMEOUT_US 100000
#define RC_CONNECTION_TOLERANCE_US 5
#define RC_ISR_MIN_US 800
#define RC_ISR_MAX_US 2200

#endif
