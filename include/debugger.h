#pragma once

#include "rc_handler.h"
#include "movement_control.h"

void init_debug_logger();
void log_rpm_history(float rpm, uint16_t throttle);
void save_rpm_history_to_flash();
void clear_saved_rpm_history();
void print_saved_rpm_history_from_flash();
void handle_debug_controls(uint16_t ch4_us, bool spin_active);

void scan_i2c();

void echo_diagnostics(const RcInput& input, const TranslationVector& translation_vector, const SpinCommand& spin_command);