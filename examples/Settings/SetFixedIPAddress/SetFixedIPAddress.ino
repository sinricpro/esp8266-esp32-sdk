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
// #define ENABLE_DEBUG

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
#include "SinricProSwitch.h"
#include "ArduinoJson.h"

#define WIFI_SSID ""
#define WIFI_PASS ""
#define APP_KEY ""     // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET ""  // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define SWITCH_ID ""   // Should look like "5dc1564130xxxxxxxxxxxxxx"

#define BAUD_RATE 115200  // Change baudrate to your need

#define SET_FIXED_IP_ADDRESS "pro.sinric::set.fixed.ip.address"

bool onSetModuleSetting(const String &id, const String &value) {
  // Handle module settings.
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, value);

  if (error) {
    Serial.print(F("onSetModuleSetting::deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }

  if (id == SET_FIXED_IP_ADDRESS) {
    String localIP = doc["localIP"];
    String gateway = doc["gateway"];
    String subnet = doc["subnet"];
    String dns1 = doc["dns1"] | "";
    String dns2 = doc["dns2"] | "";

    // Change your WiFi config here.
    Serial.printf("localIP:%s, gateway:%s, subnet:%s, dns1:%s, dns2:%s   \r\n", localIP.c_str(), gateway.c_str(), subnet.c_str(), dns1.c_str(), dns2.c_str());
    return true;
  } else {
    return false;
  }
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
  SinricProSwitch &mySwitch = SinricPro[SWITCH_ID];

  // setup SinricPro
  SinricPro.onConnected([]() {
    Serial.printf("Connected to SinricPro\r\n");
  });
  SinricPro.onDisconnected([]() {
    Serial.printf("Disconnected from SinricPro\r\n");
  });

  SinricPro.onSetSetting(onSetModuleSetting);
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
