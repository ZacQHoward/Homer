#pragma once

#include <Arduino.h>

namespace RcConfig {

    constexpr uint16_t RC_MIN_US = 1000;
    constexpr uint16_t RC_NEUTRAL_US = 1500;
    constexpr uint16_t RC_MAX_US = 2000;

    constexpr uint16_t RC_ISR_MIN_US = 800;
    constexpr uint16_t RC_ISR_MAX_US = 2200;

    constexpr uint32_t RC_SIGNAL_LOST_TIMEOUT_US = 100000;
    constexpr uint16_t RC_DEADBAND_US = 10;

}  // namespace RcConfig

struct RcInput {
    uint16_t ch1_us;
    uint16_t ch2_us;
    uint16_t ch3_us;
    uint16_t ch4_us;
    bool healthy;
};

void init_rc();
RcInput read_rc_input();