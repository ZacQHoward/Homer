#include <Arduino.h>

#include "homer_config.h"
#include "motor_driver.h"

// Helper function to clamp pulse widths to the valid range for the ESCs 1000-2000 (in microseconds)
static uint16_t clamp_pulse_us(uint16_t pulse_width_us) {
    if (pulse_width_us < MotorConfig::ESC_MIN_US) return MotorConfig::ESC_MIN_US;
    if (pulse_width_us > MotorConfig::ESC_MAX_US) return MotorConfig::ESC_MAX_US;
    return pulse_width_us;
}

// Helper function to convert a pulse width in microseconds to the corresponding duty cycle value for the configured PWM resolution and frequency
static uint32_t pulse_us_to_duty(uint16_t pulse_width_us) {
    pulse_width_us = clamp_pulse_us(pulse_width_us);

    // duty = pulse_width / frame_width * max_duty
    // Example at 12-bit:
    // 1000 us -> about 205
    // 1500 us -> about 307
    // 2000 us -> about 410
    return (static_cast<uint32_t>(pulse_width_us) * MotorConfig::ESC_PWM_MAX_DUTY) / MotorConfig::ESC_FRAME_US;
}

// Helper function to write a specific pulse width to a given PWM channel (in microseconds)
static void motor_write_us(uint8_t pwm_channel, uint16_t pulse_width_us) {
    uint32_t duty = pulse_us_to_duty(pulse_width_us);
    ledcWrite(pwm_channel, duty);
}

// Functions to write a specific pulse width to each motor's ESC (in microseconds)
void motor_1_write_us(uint16_t pulse_width_us) {
    motor_write_us(MotorConfig::MOTOR_1_PWM_CHANNEL, pulse_width_us);
}

void motor_2_write_us(uint16_t pulse_width_us) {
    motor_write_us(MotorConfig::MOTOR_2_PWM_CHANNEL, pulse_width_us);
}

// Functions to set each motor to neutral pulse width (stop)
void motor_1_neutral() {
    motor_1_write_us(MotorConfig::ESC_NEUTRAL_US);
}

void motor_2_neutral() {
    motor_2_write_us(MotorConfig::ESC_NEUTRAL_US);
}

// Sets both motors to neutral pulse width, effectively stopping the robot
void motors_stop() {
    motor_1_neutral();
    motor_2_neutral();
}

// Configures PWM channels and attaches pins from homer_config.h (how esp32 core handles PWM configuration and pin attachment)
void init_motors() {

    ledcSetup(MotorConfig::MOTOR_1_PWM_CHANNEL, MotorConfig::ESC_PWM_FREQUENCY_HZ, MotorConfig::ESC_PWM_RESOLUTION_BITS);
    ledcSetup(MotorConfig::MOTOR_2_PWM_CHANNEL, MotorConfig::ESC_PWM_FREQUENCY_HZ, MotorConfig::ESC_PWM_RESOLUTION_BITS);

    ledcAttachPin(HomerConfig::MOTOR_PIN1, MotorConfig::MOTOR_1_PWM_CHANNEL);
    ledcAttachPin(HomerConfig::MOTOR_PIN2, MotorConfig::MOTOR_2_PWM_CHANNEL);

    motors_stop();

}