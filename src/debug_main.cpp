#include <Arduino.h>

#include "accel_handler.h"
#include "homer_config.h"
#include "motor_driver.h"
#include "movement_control.h"
#include "rc_handler.h"
#include "debugger.h"

void setup() {

    Serial.begin(115200);
    init_rc();
    init_motors();
    init_accel();

    init_debug_logger();

}

void loop() {

    static bool was_spinning = false;

    RcInput rc_input = read_rc_input();

    if (!rc_input.healthy) {
        motors_stop();
        return;
    }

    TranslationVector translation_vector =
    get_translation_vector(
        rc_input.ch1_us,
        rc_input.ch2_us,
        MovementConfig::TRANS_VECTOR_DEADZONE,
        MovementConfig::DIFF_TRANS_VECTOR_DEADZONE
    );

    SpinCommand spin_command = get_spin_command(rc_input.ch3_us);

    handle_debug_controls(rc_input.ch4_us, spin_command.active);

    update_rpm_from_accel();

    if (spin_command.active && DebugConfig::ENABLE_RPM_HISTORY) {
        log_rpm_history(get_current_rpm(), rc_input.ch3_us);
    }

    if (spin_command.active) {
        was_spinning = true;
    } else {
        if (was_spinning) {
            save_rpm_history_to_flash();
            was_spinning = false;
        }
    }

    // echo_diagnostics(rc_input, translation_vector, spin_command);

    apply_movement(rc_input);
    
}