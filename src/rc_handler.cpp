#include <Arduino.h>
#include "homer_config.h"
#include "rc_handler.h"

struct RcChannelState {
    uint8_t pin;
    volatile uint32_t rise_time_us;
    volatile uint32_t pulse_width_us;
    volatile uint32_t last_pulse_us;
};

static RcChannelState channel_1 = {HomerConfig::LEFTRIGHT_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};
static RcChannelState channel_2 = {HomerConfig::FORBACK_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};
static RcChannelState channel_3 = {HomerConfig::THROTTLE_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};
static RcChannelState channel_4 = {HomerConfig::ORIENTATION_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};

static inline void IRAM_ATTR handle_channel_interrupt(volatile RcChannelState& channel) {
    if (digitalRead(channel.pin) == HIGH) {
        channel.rise_time_us = micros();
        return;
    }

    uint32_t now_us = micros();

    if (now_us <= channel.rise_time_us) {
        return;
    }

    uint32_t pulse_width_us = now_us - channel.rise_time_us;

    if (pulse_width_us >= RcConfig::RC_ISR_MIN_US &&
        pulse_width_us <= RcConfig::RC_ISR_MAX_US) {
        channel.pulse_width_us = pulse_width_us;
        channel.last_pulse_us = now_us;
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

static uint16_t read_channel_us(volatile RcChannelState& channel, uint32_t now_us, bool& healthy) {
    uint32_t pulse_width_us;
    uint32_t last_pulse_us;

    noInterrupts();
    pulse_width_us = channel.pulse_width_us;
    last_pulse_us = channel.last_pulse_us;
    interrupts();

    if (last_pulse_us == 0 || now_us - last_pulse_us > RcConfig::RC_SIGNAL_LOST_TIMEOUT_US) {
        healthy = false;
        return RcConfig::RC_NEUTRAL_US;
    }

    if (pulse_width_us < RcConfig::RC_MIN_US) {
        return RcConfig::RC_MIN_US;
    }

    if (pulse_width_us > RcConfig::RC_MAX_US) {
        return RcConfig::RC_MAX_US;
    }

    return static_cast<uint16_t>(pulse_width_us);
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

RcInput read_rc_input() {
    RcInput input;
    input.healthy = true;

    uint32_t now_us = micros();

    input.ch1_us = read_channel_us(channel_1, now_us, input.healthy);
    input.ch2_us = read_channel_us(channel_2, now_us, input.healthy);
    input.ch3_us = read_channel_us(channel_3, now_us, input.healthy);
    input.ch4_us = read_channel_us(channel_4, now_us, input.healthy);

    return input;
}
