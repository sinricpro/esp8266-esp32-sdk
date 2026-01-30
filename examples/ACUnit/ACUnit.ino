/*
 * Example for how to use the AC Unit
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
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#elif defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_MINIMA)
  #include <WiFiS3.h>
  #ifndef SINRICPRO_NOSSL
    #define SINRICPRO_NOSSL
  #endif
#endif

#include "SinricPro.h"
#include "SinricProWindowAC.h"

#define WIFI_SSID         "YOUR_WIFI_SSID"    
#define WIFI_PASS         "YOUR_WIFI_PASSWORD"
#define APP_KEY           "YOUR_APP_KEY_HERE"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "YOUR_APP_SECRET_HERE"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define ACUNIT_ID         "YOUR_DEVICE_ID_HERE"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         115200                     // Change baudrate to your need

float globalTemperature;
bool globalPowerState;
int globalFanSpeed;

bool onPowerState(const String &deviceId, bool &state) {
  SINRICPRO_PRINTF("Thermostat %s turned %s\r\n", deviceId.c_str(), state?"on":"off");
  globalPowerState = state; 
  return true; // request handled properly
}

bool onTargetTemperature(const String &deviceId, float &temperature) {
  SINRICPRO_PRINTF("Thermostat %s set temperature to %f\r\n", deviceId.c_str(), temperature);
  globalTemperature = temperature;
  return true;
}

bool onAdjustTargetTemperature(const String & deviceId, float &temperatureDelta) {
  globalTemperature += temperatureDelta;  // calculate absolut temperature
  SINRICPRO_PRINTF("Thermostat %s changed temperature about %f to %f", deviceId.c_str(), temperatureDelta, globalTemperature);
  temperatureDelta = globalTemperature; // return absolut temperature
  return true;
}

bool onThermostatMode(const String &deviceId, String &mode) {
  SINRICPRO_PRINTF("Thermostat %s set to mode %s\r\n", deviceId.c_str(), mode.c_str());
  return true;
}

bool onRangeValue(const String &deviceId, int &rangeValue) {
  SINRICPRO_PRINTF("Fan speed set to %d\r\n", rangeValue);
  globalFanSpeed = rangeValue;
  return true;
}

bool onAdjustRangeValue(const String &deviceId, int &valueDelta) {
  globalFanSpeed += valueDelta;
  SINRICPRO_PRINTF("Fan speed changed about %d to %d\r\n", valueDelta, globalFanSpeed);
  valueDelta = globalFanSpeed;
  return true;
}

void setupWiFi() {
  SINRICPRO_PRINTF("\r\n[Wifi]: Connecting");

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP); 
    WiFi.setAutoReconnect(true);
  #elif defined(ESP32)
    WiFi.setSleep(false); 
    WiFi.setAutoReconnect(true);
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASS);  

  while (WiFi.status() != WL_CONNECTED) {
    SINRICPRO_PRINTF(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  SINRICPRO_PRINTF("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  SinricProWindowAC &myAcUnit = SinricPro[ACUNIT_ID];
  myAcUnit.onPowerState(onPowerState);
  myAcUnit.onTargetTemperature(onTargetTemperature);
  myAcUnit.onAdjustTargetTemperature(onAdjustTargetTemperature);
  myAcUnit.onThermostatMode(onThermostatMode);
  myAcUnit.onRangeValue(onRangeValue);
  myAcUnit.onAdjustRangeValue(onAdjustRangeValue);

  // setup SinricPro
  SinricPro.onConnected([](){ SINRICPRO_PRINTF("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ SINRICPRO_PRINTF("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE); SINRICPRO_PRINTF("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
