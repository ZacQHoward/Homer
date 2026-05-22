#include <Arduino.h>

#include "motor_driver.h"
#include "movement_control.h"
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

    TranslationVector translation_vector = get_translation_vector(rc_input.ch1_us, rc_input.ch2_us);
    SpinCommand spin_command = get_spin_command(rc_input.ch3_us);

    apply_movement(spin_command, translation_vector);
    
}
