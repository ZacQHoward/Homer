// Code to make the robot MOVE IT MOVE IT!!
#include <Arduino.h>
#include <math.h>

#include "accel_handler.h"
#include "homer_config.h"
#include "motor_driver.h"
#include "movement_control.h"
#include "rc_handler.h"

// Helper function to convert raw RC input channels into a translation vector with x/y components, magnitude, and angle
TranslationVector get_translation_vector(uint16_t ch1_us, uint16_t ch2_us) {

    TranslationVector vec;

    // Normalize RC input channels (-1.0 to 1.0), clamp upper and lower bounds
    float ch1_normalized = (static_cast<float>(ch1_us) - static_cast<float>(RcConfig::RC_NEUTRAL_US)) / RcConfig::RC_RANGE_US;
    if (ch1_normalized > 1.0f) { ch1_normalized = 1.0f; }
    if (ch1_normalized < -1.0f) { ch1_normalized = -1.0f; }

    float ch2_normalized = (static_cast<float>(ch2_us) - static_cast<float>(RcConfig::RC_NEUTRAL_US)) / RcConfig::RC_RANGE_US;
    if (ch2_normalized > 1.0f) { ch2_normalized = 1.0f; }
    if (ch2_normalized < -1.0f) { ch2_normalized = -1.0f; }

    vec.x = ch2_normalized;   // +X = forward
    vec.y = -ch1_normalized;  // +Y = left

    // Apply deadzone
    if (fabs(vec.x) < MovementConfig::TRANS_VECTOR_DEADZONE) { vec.x = 0.0f; }
    if (fabs(vec.y) < MovementConfig::TRANS_VECTOR_DEADZONE) { vec.y = 0.0f; }

    // Get vector magnitude and normalize x/y if diagonal input exceeds length 1.0f
    vec.magnitude = sqrt((vec.x * vec.x) + (vec.y * vec.y));

    if (vec.magnitude > 1.0f) {
        vec.x /= vec.magnitude;
        vec.y /= vec.magnitude;
        vec.magnitude = 1.0f;
    }

    // Calculate angle in degrees (0 = forward, 90 = left, 180 = backward, 270 = right)
    if (vec.magnitude <= 0.0f) {
        vec.angle_deg = 0.0f;
    } else {
        vec.angle_deg = atan2(vec.y, vec.x) * 180.0f / PI;
        if (vec.angle_deg < 0.0f) { vec.angle_deg += 360.0f; }
    }

    return vec;
}

// Helper function to convert throttle channel input into a spin command with normalized throttle and active flag
SpinCommand get_spin_command(uint16_t ch3_us) {

    SpinCommand spin;

    // Normalize throttle input (-1.0 to 1.0), clamp upper and lower bounds
    spin.throttle = (static_cast<float>(ch3_us) - static_cast<float>(RcConfig::RC_NEUTRAL_US)) / RcConfig::RC_RANGE_US;
    if (spin.throttle > 1.0f) { spin.throttle = 1.0f; }
    if (spin.throttle < -1.0f) { spin.throttle = -1.0f; }

    // Apply deadzone
    if (fabs(spin.throttle) < MovementConfig::THROTTLE_DEADZONE) { spin.throttle = 0.0f; }

    // Set active flag if throttle is non zero after deadzone
    spin.active = fabs(spin.throttle) != 0.0f;

    return spin;

}

