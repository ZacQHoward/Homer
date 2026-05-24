#pragma once

#include <Arduino.h>

namespace HomerConfig {
    constexpr float BASE_SPIN_MAX_OFFSET_PERCENTAGE = 0.4f;

    constexpr uint8_t MOTOR_PIN1 = D0;
    constexpr uint8_t MOTOR_PIN2 = D1;

    constexpr uint8_t LEFTRIGHT_RC_CHANNEL_PIN = D7;
    constexpr uint8_t FORBACK_RC_CHANNEL_PIN = D8;
    constexpr uint8_t THROTTLE_RC_CHANNEL_PIN = D9;
    constexpr uint8_t ORIENTATION_RC_CHANNEL_PIN = D10;

}  // namespace HomerConfig

namespace MotorConfig {

    constexpr uint16_t ESC_MIN_US = 1000;
    constexpr uint16_t ESC_NEUTRAL_US = 1500;
    constexpr uint16_t ESC_MAX_US = 2000;
    constexpr float ESC_RANGE_US = static_cast<float>(ESC_MAX_US - ESC_NEUTRAL_US);

    constexpr uint32_t ESC_PWM_FREQUENCY_HZ = 50;
    constexpr uint32_t ESC_FRAME_US = 1000000UL / ESC_PWM_FREQUENCY_HZ;
    constexpr uint8_t ESC_PWM_RESOLUTION_BITS = 12;
    constexpr uint32_t ESC_PWM_MAX_DUTY = (1UL << ESC_PWM_RESOLUTION_BITS) - 1UL;

    constexpr uint8_t MOTOR_1_PWM_CHANNEL = 0;
    constexpr uint8_t MOTOR_2_PWM_CHANNEL = 1;

}  // namespace MotorConfig

namespace MovementConfig {

    constexpr float THROTTLE_DEADZONE = 0.02f;
    constexpr float TRANS_VECTOR_DEADZONE = 0.2f;

}  // namespace MovementConfig

namespace RcConfig {

    constexpr uint16_t RC_MIN_US = 1000;
    constexpr uint16_t RC_NEUTRAL_US = 1500;
    constexpr uint16_t RC_MAX_US = 2000;
    constexpr float RC_RANGE_US = static_cast<float>(RC_MAX_US - RC_NEUTRAL_US);

    constexpr uint16_t RC_ISR_MIN_US = 800;
    constexpr uint16_t RC_ISR_MAX_US = 2200;

    constexpr uint32_t RC_SIGNAL_LOST_TIMEOUT_US = 100000;

}  // namespace RcConfig
