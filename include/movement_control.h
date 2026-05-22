#pragma once

#include <stdint.h>

struct TranslationVector {
    float x;          // right = positive
    float y;          // backward = positive
    float magnitude;  // magnitude of the translation vector (0.0 to 1.0)
    float angle_deg;  // angle in degrees, where 0 is right, 90 is backward, 180 is left, and 270 is forward
};

struct SpinCommand {
    float throttle;   // -1.0 to 1.0
    bool active;      // true if throttle is non zero after applying deadzone, false if within deadzone
};

TranslationVector get_translation_vector(uint16_t ch1_us, uint16_t ch2_us);
SpinCommand get_spin_command(uint16_t ch3_us);

void apply_movement(const SpinCommand& spin_command, const TranslationVector& translation_vector);
