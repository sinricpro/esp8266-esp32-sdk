/*
 * Example for how to use SinricPro Primary and Secondary WiFi settings with ESP buit-in WiFiMulti.
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
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG_ESP_PORT Serial
#define NODEBUG_WEBSOCKETS
#define NDEBUG
#endif

#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <LittleFS.h>
#include <ArduinoJson.h>

#include <SinricPro.h>
#include <SinricProSwitch.h>
#include "SinricProWiFiSettings.h"

#define APP_KEY ""     // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET ""  // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define SWITCH_ID ""   // Should look like "5dc1564130xxxxxxxxxxxxxx"

#define BAUD_RATE 115200  // Change baudrate to your need

#define SET_WIFI_PRIMARY "pro.sinric::set.wifi.primary"
#define SET_WIFI_SECONDARY "pro.sinric::set.wifi.secondary"

const bool formatLittleFSIfFailed = true;

const char* primarySSID = "";        // Set to your primary wifi's ssid
const char* primaryPassword = "";    // Set to your primary wifi's password
const char* secondarySSID = "";      // Set to your secondary wifi's ssid
const char* secondaryPassword = "";  // Set to your secondary wifi's password

SinricProWiFiSettings spws(LittleFS, primarySSID, primaryPassword, secondarySSID, secondaryPassword, "/wificonfig.dat");

bool onSetModuleSetting(const String& id, const String& value) {
  // Handle module settings.

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, value);

  if (error) {
    Serial.print(F("onSetModuleSetting::deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }

  const char* password = doc["password"].as<const char*>();
  const char* ssid = doc["ssid"].as<const char*>();
  bool connectNow = doc["connectNow"] | false;

  if (id == SET_WIFI_PRIMARY) {  // Set primary WiFi
    spws.updatePrimarySettings(ssid, password);
  } else if (id == SET_WIFI_SECONDARY) {  // Set secondary WiFi
    spws.updateSecondarySettings(ssid, password);
  }
  
  return connectNow ? connectToWiFi(ssid, password) : true;
}

bool setupLittleFS() {
#if defined(ESP8266)
  if (!LittleFS.begin() && formatLittleFSIfFailed) {
#elif defined(ESP32)
  if (!LittleFS.begin(true) && formatLittleFSIfFailed) {
#endif
    Serial.println("Formatting LittleFS...");
    if (LittleFS.format()) {
      Serial.println("LittleFS formatted successfully!");
      return true;
    } else {
      Serial.println("LittleFS formatting error!");
      return false;
    }
  } else {
    Serial.println("LittleFS Mount successful");
    return true;
  }
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  // Load settings from file or using defaults if loading fails.
  spws.begin();

  auto& settings = spws.getWiFiSettings();
  bool connected = false;

  if (spws.isValidSetting(settings.primarySSID, settings.primaryPassword)) {
    connected = connectToWiFi(settings.primarySSID, settings.primaryPassword);
  }

  if (!connected && spws.isValidSetting(settings.secondarySSID, settings.secondaryPassword)) {
    connected = connectToWiFi(settings.secondarySSID, settings.secondaryPassword);
  }

  if (connected) {
    Serial.println("Connected to WiFi!");
  } else {
    Serial.println("Failed to connect to WiFi!");
  }
}

bool connectToWiFi(const char* ssid, const char* password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.disconnect();
  delay(10);

#if defined(ESP32)
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
#elif defined(ESP8266)
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.begin(ssid, password);
#endif

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 30) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  Serial.println();

  return WiFi.status() == WL_CONNECTED;
}

// setup function for SinricPro
void setupSinricPro() {
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];

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
  setupLittleFS();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  if (WiFi.status() != WL_CONNECTED) ESP.restart();
}
