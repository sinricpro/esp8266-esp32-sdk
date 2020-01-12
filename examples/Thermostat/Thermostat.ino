/*
 * Example for how to use Lock device as garage door opener
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
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProThermostat.h"

#define WIFI_SSID         "YOUR_WIFI_SSID"    
#define WIFI_PASS         "YOUR_WIFI_PASSWORD"
#define APP_KEY           "YOUR_APP_KEY_HERE"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "YOUR_APP_SECRET_HERE"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define THERMOSTAT_ID     "YOUR_DEVICE_ID_HERE"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                     // Change baudrate to your need

float globalTemperature;
bool globalPowerState;

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Thermostat %s turned %s\r\n", deviceId.c_str(), state?"on":"off");
  globalPowerState = state; 
  return true; // request handled properly
}

bool onTargetTemperature(const String &deviceId, float &temperature) {
  Serial.printf("Thermostat %s set temperature to %f\r\n", deviceId.c_str(), temperature);
  globalTemperature = temperature;
  return true;
}

bool onAdjustTargetTemperature(const String & deviceId, float &temperatureDelta) {
  globalTemperature += temperatureDelta;  // calculate absolut temperature
  Serial.printf("Thermostat %s changed temperature about %f to %f", deviceId.c_str(), temperatureDelta, globalTemperature);
  temperatureDelta = globalTemperature; // return absolut temperature
  return true;
}

bool onThermostatMode(const String &deviceId, String &mode) {
  Serial.printf("Thermostat %s set to mode %s\r\n", deviceId.c_str(), mode.c_str());
  return true;
}


void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  SinricProThermostat &myThermostat = SinricPro[THERMOSTAT_ID];
  myThermostat.onPowerState(onPowerState);
  myThermostat.onTargetTemperature(onTargetTemperature);
  myThermostat.onAdjustTargetTemperature(onAdjustTargetTemperature);
  myThermostat.onThermostatMode(onThermostatMode);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}