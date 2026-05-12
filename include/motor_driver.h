#pragma once

#include <Arduino.h>

namespace MotorConfig {

    constexpr uint16_t ESC_MIN_US = 1000;      // Minimum pulse width for ESC (full reverse in microseconds)
    constexpr uint16_t ESC_NEUTRAL_US = 1500;  // Neutral pulse width for ESC (stop in microseconds)
    constexpr uint16_t ESC_MAX_US = 2000;      // Maximum pulse width for ESC (full forward in microseconds)

    constexpr uint32_t ESC_PWM_FREQUENCY_HZ = 50;   // Increase for faster ESC command updates and potentially smoother response, Higher values may reduce compatibility with some ESCs
    constexpr uint32_t ESC_FRAME_US = 1000000UL / ESC_PWM_FREQUENCY_HZ;  // Frame width in microseconds based on configured frequency (e.g. 20000 us for 50 Hz)
    constexpr uint8_t ESC_PWM_RESOLUTION_BITS = 12; // PWM duty resolution, Higher values allow finer PWM pulse precision but very high resolutions may limit maximum achievable PWM frequency
    constexpr uint32_t ESC_PWM_MAX_DUTY = (1UL << ESC_PWM_RESOLUTION_BITS) - 1UL;   // Maximum duty cycle value based on configured PWM resolution (e.g. 4095 for 12-bit resolution)

    constexpr uint8_t MOTOR_1_PWM_CHANNEL = 0;  // LEDC channel slop
    constexpr uint8_t MOTOR_2_PWM_CHANNEL = 1;  // LEDC channel slop

}  // namespace MotorConfig

void init_motors();

void motor_1_write_us(uint16_t pulse_width_us);
void motor_2_write_us(uint16_t pulse_width_us);

void motor_1_neutral();
void motor_2_neutral();
void motors_stop();