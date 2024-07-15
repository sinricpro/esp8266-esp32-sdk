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
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG_ESP_PORT Serial
#define NODEBUG_WEBSOCKETS
#define NDEBUG
#endif

#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;  ///< ESP8266 WiFi multi instance.
#elif defined(ESP32)
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;  ///< ESP32 WiFi multi instance.
#endif

#include "FS.h"
#include "LittleFS.h"
#include "ArduinoJson.h"

#include "SinricPro.h"
#include "SinricProSwitch.h"
#include "SinricProWiFiSettings.h"

#define APP_KEY ""        // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET ""     // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define SWITCH_ID ""      // Should look like "5dc1564130xxxxxxxxxxxxxx"

#define BAUD_RATE 115200  // Change baudrate to your need

#define SET_WIFI_PRIMARY "pro.sinric::set.wifi.primary"
#define SET_WIFI_SECONDARY "pro.sinric::set.wifi.secondary"

const bool formatLittleFSIfFailed = true;
const unsigned long NO_WIFI_REBOOT_TIMEOUT = 300000;  // 5 minutes in milliseconds
unsigned long wifiStartTime;

const char* primary_ssid = "";       // Set to your primary wifi's ssid
const char* primary_password = "";   // Set to your primary wifi's password
const char* secondary_ssid = "";     // Set to your secondary wifi's ssid
const char* secondary_password = ""; // Set to your secondary wifi's password

SinricProWiFiSettings spws(primary_ssid, primary_password, secondary_ssid, secondary_password, "/wificonfig.dat");

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

  if (id == SET_WIFI_PRIMARY) {  // Set primary WiFi
    spws.updatePrimarySettings(ssid, password);
  } else if (id == SET_WIFI_SECONDARY) {  // Set secondary WiFi
    spws.updateSecondarySettings(ssid, password);
  }

  bool connect = doc["connectNow"] | false;
  if (connect) {
    #if defined(ESP8266)
      wifiMulti.cleanAPlist();
    #elif defined(ESP32)
      wifiMulti.APlistClean();
    #endif

    wifiMulti.addAP(ssid, password);
    return waitForConnectResult();
  }

  return true;
}

bool setupLittleFS() {
  // Sets up the LittleFS.
  #if defined(ESP8266)
    if (!LittleFS.begin()) {
  #elif defined(ESP32)
    if (!LittleFS.begin(true)) {
  #endif

    Serial.println("An Error has occurred while mounting LittleFS");

    if (formatLittleFSIfFailed) {
      Serial.println("Formatting LittleFS...");
      if (LittleFS.format()) {
        Serial.println("LittleFS formatted successfully!");
        return true;
      } else {
        Serial.println("LittleFS formatting error!");
        return false;
      }
    } else {
      Serial.println("LittleFS error!");
      return false;
    }
  } else {
    Serial.println("LittleFS Mount successful");
    return true;
  }

  return true;
}

bool waitForConnectResult() {
  unsigned long startTime = millis();
  constexpr unsigned int connectTimeout = 10000;

  Serial.println("Connecting Wifi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (millis() - startTime >= connectTimeout) {
      Serial.println("WIFI not connected");
      return false;
    }
  }

  Serial.printf("\nWiFi connected\nIP address: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  WiFi.mode(WIFI_STA);
#if defined(ESP8266)
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
#elif defined(ESP32)
  WiFi.setSleep(false);
#endif
  WiFi.setAutoReconnect(true);

  // Load settings from file or using defaults if loading fails.
  spws.begin();

  const SinricProWiFiSettings::wifi_settings_t& settings = spws.getWiFiSettings();

  if (spws.isValidSetting(settings.primarySSID, settings.primaryPassword)) {
    wifiMulti.addAP(settings.primarySSID, settings.primaryPassword);
  }

  if (spws.isValidSetting(settings.secondarySSID, settings.secondaryPassword)) {
    wifiMulti.addAP(settings.secondarySSID, settings.secondaryPassword);
  }

  waitForConnectResult();
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

void rebootIfNoWiFi() {
  // If no WiFI connection for 5 mins reboot the ESP. ESP will connect to either primary or secondary based on availability
  if (WiFi.status() != WL_CONNECTED && (millis() - wifiStartTime >= NO_WIFI_REBOOT_TIMEOUT)) {
    Serial.println("WiFi connection timed out. Rebooting...");
    ESP.restart();
  } else {
    // Reset the start time if WiFi is connected
    wifiStartTime = millis();
  }
}

void loop() {
  wifiMulti.run();
  SinricPro.handle();
  rebootIfNoWiFi();
}
