#include <Arduino.h>

#include "accel_handler.h"
#include "debugger.h"
#include "homer_config.h"
#include "motor_driver.h"
#include "movement_control.h"
#include "rc_handler.h"

static void echo_diagnostics(const RcInput& input, const TranslationVector& translation_vector, const SpinCommand& spin_command) {

    Serial.print("  RC Health: ");
    Serial.print(input.healthy);
    Serial.print("  CH1: ");
    Serial.print(input.ch1_us);
    Serial.print("  CH2: ");
    Serial.print(input.ch2_us);
    Serial.print("  CH3: ");
    Serial.print(input.ch3_us);
    Serial.print("  CH4: ");
    Serial.print(input.ch4_us);

    Serial.print("  TVec X: ");
    Serial.print(translation_vector.x, 3);
    Serial.print("  TVec Y: ");
    Serial.print(translation_vector.y, 3);
    Serial.print("  TVec Mag: ");
    Serial.print(translation_vector.magnitude, 3);
    Serial.print("  TVec Angle: ");
    Serial.print(translation_vector.angle_deg, 1);

    Serial.print("  Spin Throttle: ");
    Serial.print(spin_command.throttle, 3);
    Serial.print("  Spin Active: ");
    Serial.print(spin_command.active);

    Serial.print("  Raw Accel G: ");
    Serial.print(get_accel_force_g(), 4);
    Serial.print("  Current RPM: ");
    Serial.print(get_current_rpm());
    Serial.print("  Max RPM: ");
    Serial.print(get_max_rpm());
    Serial.println();

}

void setup() {

    Serial.begin(115200);

    init_rc();
    init_motors();
    init_accel();

    reset_rpm_history();

}

void loop() {

    RcInput rc_input = read_rc_input();
    TranslationVector translation_vector = get_translation_vector(rc_input.ch1_us, rc_input.ch2_us);
    SpinCommand spin_command = get_spin_command(rc_input.ch3_us);

    update_rpm_from_accel();
    if (spin_command.active && DebugConfig::ENABLE_RPM_HISTORY) { log_rpm_history(spin_command); }

    echo_diagnostics(rc_input, translation_vector, spin_command);

    if (!rc_input.healthy) {
        motors_stop();
        return;
    }

    if (!spin_command.active) {
        motors_stop();
        print_rpm_history();
        delay(3000);
        return;
    }

    apply_spin_only_test(spin_command);

}