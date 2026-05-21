#include <Arduino.h>

#include "motor_driver.h"
#include "rc_handler.h"

void setup() {
    Serial.begin(115200);
    init_rc();
    init_motors();
}

void loop() {
    RcInput rc_input = read_rc_input();

    if (!rc_input.healthy) {
        motors_stop();
        return;
    }

    motor_1_write_us(rc_input.ch3_us);
    motor_2_write_us(rc_input.ch2_us);
}
