/*
 * Example for how to use SinricPro Settings:
 * 
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG_ESP_PORT Serial
#define NODEBUG_WEBSOCKETS
#define NDEBUG
#endif

#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
#include <WiFi.h>
#endif

#include "SinricPro.h"
#include "ArduinoJson.h"

#define WIFI_SSID "YOUR-WIFI-SSID"
#define WIFI_PASS "YOUR-WIFI-PASSWORD"
#define APP_KEY "YOUR-APP-KEY"        // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "YOUR-APP-SECRET"  // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define SWITCH_ID "YOUR-DEVICE-ID"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE 115200              // Change baudrate to your need

#define SET_WIFI "pro.sinric::set.wifi"
#define GET_CONNECTED_WIFI_INFO "pro.sinric::get.connected.wifi.info"

struct WiFiInfo {
  int32_t channel;
  long rssi;
  String encryptionType;
};

bool handleSetSetting(const String& id, const String& value) {
  if (id == SET_WIFI) {
    // Connect to another WiFi
    // JsonDocument doc;
    // deserializeJson(doc, value);
    // const char* ssid = doc["ssid"];
    // const char* password = doc["password"];
  }

  return true;
}

WiFiInfo getWiFiInfo(const char* ssid) {
  struct WiFiInfo info;

  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        info.channel = WiFi.channel(i);
        info.rssi = WiFi.RSSI(i);

        switch (WiFi.encryptionType(i)) {
          case WIFI_AUTH_OPEN: info.encryptionType = "open"; break;
          case WIFI_AUTH_WEP: info.encryptionType = "WEP"; break;
          case WIFI_AUTH_WPA_PSK: info.encryptionType = "WPA"; break;
          case WIFI_AUTH_WPA2_PSK: info.encryptionType = "WPA2"; break;
          case WIFI_AUTH_WPA_WPA2_PSK: info.encryptionType = "WPA+WPA2"; break;
          case WIFI_AUTH_WPA2_ENTERPRISE: info.encryptionType = "WPA2-EAP"; break;
          case WIFI_AUTH_WPA3_PSK: info.encryptionType = "WPA3"; break;
          case WIFI_AUTH_WPA2_WPA3_PSK: info.encryptionType = "WPA2+WPA3"; break;
          case WIFI_AUTH_WAPI_PSK: info.encryptionType = "WAPI"; break;
          default: info.encryptionType = "unknown"; break;
        }
      }
    }
  }

  // Delete the scan result to free memory
  WiFi.scanDelete();

  return info;
}

bool handleGetSetting(const String& id, String& value) {
  if (id == GET_CONNECTED_WIFI_INFO) {
    WiFiInfo info = getWiFiInfo(WIFI_SSID);

    JsonDocument doc;
    doc["rssi"] = info.rssi;
    doc["channel"] = info.channel;
    doc["ssid"] = info.encryptionType;

    serializeJson(doc, value);
  }

  return true;
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

#if defined(ESP8266)
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect(true);
#elif defined(ESP32)
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
#endif

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// setup function for SinricPro
void setupSinricPro() {

  // setup SinricPro
  SinricPro.onConnected([]() {
    Serial.printf("Connected to SinricPro\r\n");
  });
  SinricPro.onDisconnected([]() {
    Serial.printf("Disconnected from SinricPro\r\n");
  });
  SinricPro.onSetSetting(handleSetSetting);
  SinricPro.onGetSetting(handleGetSetting);
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
