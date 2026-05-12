#pragma once

#include <Arduino.h>

namespace RcConfig {

    constexpr uint16_t RC_MIN_US = 1000;      // Minimum clamp value for RC input pulse width (in microseconds)
    constexpr uint16_t RC_NEUTRAL_US = 1500;  // Neutral pulse width for RC input (in microseconds)
    constexpr uint16_t RC_MAX_US = 2000;      // Maximum clamp value for RC input pulse width (in microseconds)

    constexpr uint16_t RC_ISR_MIN_US = 800;   // Minimum valid pulse width for RC signal (in microseconds)
    constexpr uint16_t RC_ISR_MAX_US = 2200;  // Maximum valid pulse width for RC signal (in microseconds)
    constexpr uint16_t RC_DEADBAND_US = 10;   // Deadband for RC input channels to prevent noise around neutral from causing movement

    constexpr uint32_t RC_SIGNAL_LOST_TIMEOUT_US = 100000;  // Timeout for RC signal loss detection (in microseconds)

}  // namespace RcConfig

// Represents the current state of the RC input channels and whether the signal is healthy
struct RcInput {
    uint16_t ch1_us;
    uint16_t ch2_us;
    uint16_t ch3_us;
    uint16_t ch4_us;
    bool healthy;
};

void init_rc(); // Setup RC input pins and interrupts
RcInput read_rc_input();    // Read RC input values, check for signal health, and return as RcInput struct