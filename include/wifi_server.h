#ifndef HOMER_WIFI_SERVER_H
#define HOMER_WIFI_SERVER_H

#include <Arduino.h>

struct telemetry_data_t {
  uint32_t sample_ms;
  float raw_g;
  float zeroed_g;
  float avg_zeroed_g;
  int16_t x;
  int16_t y;
  int16_t z;
  float offset;
  uint16_t ch1_us;
  uint16_t ch2_us;
  uint16_t ch3_us;
  uint16_t ch4_us;
  bool controller_connected;
  bool rc_healthy;
};

void init_wifi_dashboard(const char *ssid, const char *password);
void wifi_dashboard_loop();
void wifi_dashboard_set_data(const telemetry_data_t &data);

#endif
