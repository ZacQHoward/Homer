//This file interfaces with the RC receiver to get channel pulse widths via interrupts while checking for signal health based on time since last valid pulse
//It also clamps pulse widths to expected min/max values to prevent issues from bad signals and returns the current RC input state as an RcInput struct for use in the main control loop

#include <Arduino.h>
#include "homer_config.h"
#include "rc_handler.h"

// Internal struct to keep track of each RC channel's state for interrupt handling
struct RcChannelState {
    uint8_t pin;
    volatile uint32_t rise_time_us;
    volatile uint32_t pulse_width_us;
    volatile uint32_t last_pulse_us;
};

// Initialize channel states for the 4 RC channels based on pin mappings defined in homer_config.h and neutral pulse widths
static RcChannelState channel_1 = {HomerConfig::LEFTRIGHT_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};
static RcChannelState channel_2 = {HomerConfig::FORBACK_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};
static RcChannelState channel_3 = {HomerConfig::THROTTLE_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};
static RcChannelState channel_4 = {HomerConfig::ORIENTATION_RC_CHANNEL_PIN, 0, RcConfig::RC_NEUTRAL_US, 0};

// Common handler for RC channel interrupts that measures pulse width and updates channel state
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

// Interrupt handlers for each RC channel that call the common handler with the appropriate channel state
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

// Helper function to read a channel's pulse width, check for signal health, and clamp to min/max values
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

// Initializes RC input pins and attaches interrupts that handle pulse width measurement
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

// Reads RC input values, returns the current state and health as an RcInput struct
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
