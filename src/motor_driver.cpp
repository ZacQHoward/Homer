// This file interfaces with the motors to provide functions for initializing, stopping, and writing normalized motor commands which are converted to ESC pulse widths

#include "motor_driver.h"
#include <Arduino.h>
#include "homer_config.h"

// Helper function to convert normalized input (-1.0 to 1.0) into ESC pulse width in microseconds to write to the motors
static uint16_t normalized_to_pulse_us(float input) {
    if (input > 1.0f) input = 1.0f;
    if (input < -1.0f) input = -1.0f;

    int16_t pulse_width_us = MotorConfig::ESC_NEUTRAL_US + static_cast<int16_t>(input * MotorConfig::ESC_RANGE_US);

    if (pulse_width_us < MotorConfig::ESC_MIN_US) return MotorConfig::ESC_MIN_US;
    if (pulse_width_us > MotorConfig::ESC_MAX_US) return MotorConfig::ESC_MAX_US;

    return static_cast<uint16_t>(pulse_width_us);
}

// Function to write a specific pulse width in microseconds to a motor's PWM channel, with clamping to min/max values
void write_motor_us(uint8_t pwm_channel, uint16_t pulse_width_us) {
    if (pulse_width_us < MotorConfig::ESC_MIN_US) pulse_width_us = MotorConfig::ESC_MIN_US;
    if (pulse_width_us > MotorConfig::ESC_MAX_US) pulse_width_us = MotorConfig::ESC_MAX_US;

    uint32_t duty = (static_cast<uint32_t>(pulse_width_us) * MotorConfig::ESC_PWM_MAX_DUTY) / MotorConfig::ESC_FRAME_US;

    ledcWrite(pwm_channel, duty);
}

// Public function to write normalized motor inputs, which converts to pulse widths and writes to each motor
void motors_write_normalized(float motor_1_input, float motor_2_input) {
    write_motor_us(MotorConfig::MOTOR_1_PWM_CHANNEL, normalized_to_pulse_us(motor_1_input));
    write_motor_us(MotorConfig::MOTOR_2_PWM_CHANNEL, normalized_to_pulse_us(motor_2_input));
}

// Public function to stop the motors by writing the neutral pulse width to both motor channels
void motors_stop() {
    write_motor_us(MotorConfig::MOTOR_1_PWM_CHANNEL, MotorConfig::ESC_NEUTRAL_US);
    write_motor_us(MotorConfig::MOTOR_2_PWM_CHANNEL, MotorConfig::ESC_NEUTRAL_US);
}

// Configures PWM channels and attaches pins from homer_config.h (how esp32 core handles PWM configuration and pin attachment)
void init_motors() { 

    ledcSetup(MotorConfig::MOTOR_1_PWM_CHANNEL, MotorConfig::ESC_PWM_FREQUENCY_HZ, MotorConfig::ESC_PWM_RESOLUTION_BITS);
    ledcSetup(MotorConfig::MOTOR_2_PWM_CHANNEL, MotorConfig::ESC_PWM_FREQUENCY_HZ, MotorConfig::ESC_PWM_RESOLUTION_BITS);

    ledcAttachPin(HomerConfig::MOTOR_PIN1, MotorConfig::MOTOR_1_PWM_CHANNEL);
    ledcAttachPin(HomerConfig::MOTOR_PIN2, MotorConfig::MOTOR_2_PWM_CHANNEL);

    motors_stop();

}