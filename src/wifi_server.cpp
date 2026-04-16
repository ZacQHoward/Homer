#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "wifi_server.h"
#include "webpage.h"

static WebServer server(80);
static telemetry_data_t latest_data = {};

static const char *ACTIVE_SSID = nullptr;
static const char *ACTIVE_PASSWORD = nullptr;

static void handle_root() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", dashboard_html());
}

static void handle_data() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.sendHeader("Pragma", "no-cache");
  String json = "{";
  json += "\"sample_ms\":" + String(latest_data.sample_ms) + ",";
  json += "\"raw_g\":" + String(latest_data.raw_g, 4) + ",";
  json += "\"zeroed_g\":" + String(latest_data.zeroed_g, 4) + ",";
  json += "\"avg_zeroed_g\":" + String(latest_data.avg_zeroed_g, 4) + ",";
  json += "\"x\":" + String(latest_data.x) + ",";
  json += "\"y\":" + String(latest_data.y) + ",";
  json += "\"z\":" + String(latest_data.z) + ",";
  json += "\"offset\":" + String(latest_data.offset, 4) + ",";
  json += "\"ch1_us\":" + String(latest_data.ch1_us) + ",";
  json += "\"ch2_us\":" + String(latest_data.ch2_us) + ",";
  json += "\"ch3_us\":" + String(latest_data.ch3_us) + ",";
  json += "\"ch4_us\":" + String(latest_data.ch4_us) + ",";
  json += "\"controller_connected\":" + String(latest_data.controller_connected ? "true" : "false") + ",";
  json += "\"rc_healthy\":" + String(latest_data.rc_healthy ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void init_wifi_dashboard(const char *ssid, const char *password) {
  ACTIVE_SSID = ssid;
  ACTIVE_PASSWORD = password;

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ACTIVE_SSID, ACTIVE_PASSWORD);

  Serial.print("AP started. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handle_root);
  server.on("/data", handle_data);
  server.begin();
}

void wifi_dashboard_loop() {
  server.handleClient();
}

void wifi_dashboard_set_data(const telemetry_data_t &data) {
  latest_data = data;
}
