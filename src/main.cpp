#include <Arduino.h>

#include "accel_handler.h"
#include "melty_config.h"
#include "motor_driver.h"
#include "rc_handler.h"
#include "spin_control.h"

static uint16_t read_ch1() { return rc_get_ch1_pulse_us(); }
static uint16_t read_ch2() { return rc_get_ch2_pulse_us(); }
static uint16_t read_ch3() { return rc_get_ch3_pulse_us(); }
static uint16_t read_ch4() { return rc_get_ch4_pulse_us(); }

static bool throttle_is_active() {
  uint16_t ch3 = read_ch3();
  return ch3 <= 1490 || ch3 >= 1510;
}

static void echo_diagnostics() {
  Serial.print("Raw Accel G: ");
  Serial.print(get_accel_force_g(), 4);
  Serial.print("  RC Health: ");
  Serial.print(rc_signal_is_healthy());
  Serial.print("  CH1: ");
  Serial.print(read_ch1());
  Serial.print("  CH2: ");
  Serial.print(read_ch2());
  Serial.print("  CH3: ");
  Serial.print(read_ch3());
  Serial.print("  CH4: ");
  Serial.print(read_ch4());
  Serial.print("  Max RPM: ");
  Serial.print(get_max_rpm());
  Serial.println();
}

static void wait_for_rc_and_neutral_throttle() {
  while (!rc_signal_is_healthy() || throttle_is_active()) {
    motors_off();
    delay(300);
    echo_diagnostics();
  }
}

static void handle_idle() {
  motors_off();
  delay(150);
  echo_diagnostics();
}

void setup() {
  Serial.begin(115200);

  init_motors();
  init_rc();
  init_accel();

#ifdef VERIFY_RC_THROTTLE_ZERO_AT_BOOT
  wait_for_rc_and_neutral_throttle();
#endif
}

void loop() {
  if (!rc_signal_is_healthy()) {
    motors_off();
    delay(300);
    echo_diagnostics();
    return;
  }

  if (throttle_is_active()) {
    spin_one_rotation();
  } else {
    handle_idle();
  }
}
