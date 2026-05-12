#include <Arduino.h>

#include "homer_config.h"
#include "accel_handler.h"
#include "motor_driver.h"
#include "rc_handler.h"
#include "spin_control.h"

static bool throttle_is_active(const RcInput& input) {
  int centered = (int)input.ch3_us - (int)RcConfig::RC_NEUTRAL_US;
  return abs(centered) > 10;
}

static void echo_diagnostics(const RcInput& input) {
  Serial.print("Raw Accel G: ");
  Serial.print(get_accel_force_g(), 4);
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
  Serial.print("  Max RPM: ");
  Serial.print(get_max_rpm());
  Serial.println();
}

static void handle_idle(const RcInput& input) {
  motors_off();
  delay(150);
  echo_diagnostics(input);
}

void setup() {
  Serial.begin(115200);

  init_rc();
  init_motors();
  init_accel();

}

void loop() {
  RcInput input = read_rc_input();

  if (throttle_is_active(input)) {
    spin_one_rotation(input);
  } else {
    handle_idle(input);
  }

  echo_diagnostics(input);

}
