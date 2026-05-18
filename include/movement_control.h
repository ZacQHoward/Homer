#pragma once

#include <stdint.h>

struct TranslationVector {
    float x;          // forward = positive
    float y;          // left = positive
    float magnitude;  // magnitude of the translation vector (0.0 to 1.0)
    float angle_deg;  // angle in degrees, where 0 is forward, 90 is left, 180 is backward, and 270 is right
};

struct SpinCommand {
    float throttle;   // -1.0 to 1.0
    bool active;      // true if throttle is non zero after applying deadzone, false if within deadzone
};

TranslationVector get_translation_vector(uint16_t ch1_us, uint16_t ch2_us);
SpinCommand get_spin_command(uint16_t ch3_us);
float get_current_rpm();
float get_max_rpm();
void update_rpm_from_accel();
void reset_max_rpm();