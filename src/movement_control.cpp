// Code to make the robot MOVE IT MOVE IT!!
#include <Arduino.h>
#include <math.h>

#include "homer_config.h"
#include "motor_driver.h"
#include "movement_control.h"

static float clamp_normalized(float value) {
    if (value > 1.0f) return 1.0f;
    if (value < -1.0f) return -1.0f;
    return value;
}

static float normalize_rc_channel(uint16_t pulse_width_us) {
    
    float value = (static_cast<float>(pulse_width_us) - static_cast<float>(RcConfig::RC_NEUTRAL_US)) / static_cast<float>(RcConfig::RC_RANGE_US);

    return clamp_normalized(value);
}

// -- Movement Components --
// Helper function to convert raw RC input channels into a translation vector with x/y components, magnitude, and angle
TranslationVector get_translation_vector(uint16_t ch1_us, uint16_t ch2_us) {

    TranslationVector vec;

    // Normalize RC input channels (-1.0 to 1.0), clamp upper and lower bounds
    float ch1_normalized = normalize_rc_channel(ch1_us);
    float ch2_normalized = normalize_rc_channel(ch2_us);

    vec.x = ch1_normalized;  // +X = Left/Right
    vec.y = ch2_normalized; // +Y = Forward/Backward

    // Apply deadzone (temp change so left/right isnt as touchy)
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
    spin.throttle = normalize_rc_channel(ch3_us);

    // Apply deadzone
    if (fabs(spin.throttle) < MovementConfig::THROTTLE_DEADZONE) { spin.throttle = 0.0f; }

    // Set active flag if throttle is non zero after deadzone
    spin.active = spin.throttle != 0.0f;

    return spin;

}

void apply_movement(const SpinCommand& spin_command, const TranslationVector& translation_vector) {
    float spin_output = -spin_command.throttle;

    float motor_1_diff = translation_vector.x - translation_vector.y;
    float motor_2_diff = translation_vector.x + translation_vector.y;

    float motor_1_output = spin_output + motor_1_diff;
    float motor_2_output = spin_output + motor_2_diff;

    motor_1_output = clamp_normalized(motor_1_output);
    motor_2_output = clamp_normalized(motor_2_output);

    motors_write_normalized(motor_1_output, motor_2_output);
}
