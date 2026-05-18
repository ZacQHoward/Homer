#pragma once

#include <stdint.h>

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