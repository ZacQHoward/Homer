// Code to make the robot MOVE IT MOVE IT!!

#include "movement_control.h"
#include <Arduino.h>
#include <math.h>
#include "accel_handler.h"
#include "homer_config.h"
#include "motor_driver.h"
#include "rc_handler.h"

static float current_rpm = 0.0f;
static float max_rpm = 0.0f;

// -- Movement Components --
// Helper function to convert raw RC input channels into a translation vector with x/y components, magnitude, and angle
TranslationVector get_translation_vector(uint16_t ch1_us, uint16_t ch2_us, float x_deadzone, float y_deadzone) {

    TranslationVector vec;

    // Normalize RC input channels (-1.0 to 1.0), clamp upper and lower bounds
    float ch1_normalized = normalize_rc_channel(ch1_us);
    float ch2_normalized = normalize_rc_channel(ch2_us);

    vec.x = ch2_normalized; // +X = Forward/Backward
    vec.y = -ch1_normalized; // +Y = Left/Right

    // Apply deadzone
    if (fabs(vec.x) < x_deadzone) { vec.x = 0.0f; }
    if (fabs(vec.y) < y_deadzone) { vec.y = 0.0f; }

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

// -- RPM Tracking Functions --
float get_current_rpm() {
    return current_rpm;
}

float get_max_rpm() {
    return max_rpm;
}

void update_rpm_from_accel() {

    float accel_g = fabs(get_accel_force_g() - HomerConfig::DEFAULT_ACCEL_ZERO_G_OFFSET);

    current_rpm = sqrt((accel_g * 89445.0f) / HomerConfig::ACCEL_MOUNT_RADIUS_CM);
    if (current_rpm > max_rpm) { max_rpm = current_rpm; }
    
}

// -- Drive Mode Functions --
// Function for applying differential drive movement by combining translation and spin components into motor outputs and writing to motors
static void apply_diff_drive(const SpinCommand& spin_command, const TranslationVector& translation_vector) {

    // GPT Suggested DIFF
    float forward = translation_vector.x;
    float turn = translation_vector.y;

    // Negative may be needed depending on transmitter/throttle direction.
    // Lab test: CH3 up should spin the robot in your preferred spin-up direction.
    float spin = -spin_command.throttle;

    // For your mirrored motor setup:
    // same sign on both motors = spin
    // opposite signs on motors = translation/turn-style driving
    float motor_1_drive = forward + turn;
    float motor_2_drive = forward - turn;

    float motor_1_output = spin - motor_1_drive;
    float motor_2_output = spin + motor_2_drive;

    // OLD DIFF
    // // Base spin output is based on throttle (ch3, up/positive for counter-clockwise, down/negative for clockwise)
    // float spin_output = -spin_command.throttle;

    // // Calculate motor output differences based on translation vector
    // float motor_1_diff = translation_vector.x - translation_vector.y;
    // float motor_2_diff = translation_vector.x + translation_vector.y;

    // // Combine spin and translation components for each motor
    // float motor_1_output = spin_output + motor_1_diff;
    // float motor_2_output = spin_output + motor_2_diff;

    // Write normalized outputs to motors clamping is handled in motor driver
    motors_write_normalized(motor_1_output, motor_2_output);

}

// Function for applying Meltybrain drive by using the translation vector angle to determine when in the phase to spin what motor faster
static void apply_meltybrain_drive(const SpinCommand& spin_command, const TranslationVector& translation_vector) {

}

// Main function to apply movement based on configured drive mode
void apply_movement(const RcInput& rc_input) {

    SpinCommand spin_command = get_spin_command(rc_input.ch3_us);

    switch (MovementConfig::DRIVE_MODE) {

        case MovementConfig::MANUAL: {
            write_motor_us(MotorConfig::MOTOR_1_PWM_CHANNEL, rc_input.ch3_us);
            write_motor_us(MotorConfig::MOTOR_2_PWM_CHANNEL, rc_input.ch2_us);
            break;
        }

        case MovementConfig::DIFF_DRIVE: {
            TranslationVector translation_vector = get_translation_vector(rc_input.ch1_us, rc_input.ch2_us, MovementConfig::TRANS_VECTOR_DEADZONE, MovementConfig::DIFF_TRANS_VECTOR_DEADZONE);
            apply_diff_drive(spin_command, translation_vector);
            break;
        }

        case MovementConfig::MELTYBRAIN: {
            TranslationVector translation_vector = get_translation_vector(rc_input.ch1_us, rc_input.ch2_us, MovementConfig::TRANS_VECTOR_DEADZONE, MovementConfig::TRANS_VECTOR_DEADZONE);
            apply_meltybrain_drive(spin_command, translation_vector);
            break;
        }

        default: {
            motors_stop();
            break;
        }

    }

}