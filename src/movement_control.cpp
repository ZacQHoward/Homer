// Code to make the robot MOVE IT MOVE IT!!
#include <Arduino.h>
#include <math.h>

#include "accel_handler.h"
#include "homer_config.h"
#include "motor_driver.h"
#include "movement_control.h"
#include "rc_handler.h"

static float current_rpm = 0.0f;
static float max_rpm = 0.0f;

static float rpm_history[DebugConfig::RPM_HISTORY_SIZE];
static uint16_t rpm_history_index = 0;
static bool rpm_history_wrapped = false;

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

void update_rpm_from_accel() {

    float accel_g = fabs(get_accel_force_g() - HomerConfig::DEFAULT_ACCEL_ZERO_G_OFFSET);
    float radius_cm = HomerConfig::ACCEL_MOUNT_RADIUS_CM;

    current_rpm = sqrt((accel_g * 89445.0f) / radius_cm);

    if (current_rpm > max_rpm) { max_rpm = current_rpm; }

}

// RPM get functions
float get_current_rpm() { return current_rpm; }
float get_max_rpm() { return max_rpm; }

// Helper function to reset RPM history and variables
void reset_rpm_history() {

    current_rpm = 0.0f;
    max_rpm = 0.0f;
    rpm_history_index = 0;
    rpm_history_wrapped = false;

    for (uint16_t i = 0; i < DebugConfig::RPM_HISTORY_SIZE; i++) { rpm_history[i] = 0.0f; }

}

void log_rpm_history() {

    if (!DebugConfig::ENABLE_RPM_HISTORY) {
        return;
    }

    static uint32_t last_log_ms = 0;
    uint32_t now_ms = millis();

    if (now_ms - last_log_ms < DebugConfig::RPM_HISTORY_LOG_INTERVAL_MS) {
        return;
    }

    last_log_ms = now_ms;

    rpm_history[rpm_history_index] = current_rpm;
    rpm_history_index++;

    if (rpm_history_index >= DebugConfig::RPM_HISTORY_SIZE) {
        rpm_history_index = 0;
        rpm_history_wrapped = true;
    }

}

void print_rpm_history() {

    uint16_t count =
        rpm_history_wrapped ?
        DebugConfig::RPM_HISTORY_SIZE :
        rpm_history_index;

    Serial.println("RPM_HISTORY_BEGIN");

    if (rpm_history_wrapped) {

        for (uint16_t i = rpm_history_index; i < DebugConfig::RPM_HISTORY_SIZE; i++) {
            Serial.println(rpm_history[i]);
        }

        for (uint16_t i = 0; i < rpm_history_index; i++) {
            Serial.println(rpm_history[i]);
        }

    } else {

        for (uint16_t i = 0; i < count; i++) {
            Serial.println(rpm_history[i]);
        }

    }

    Serial.println("RPM_HISTORY_END");

}