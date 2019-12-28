/*
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
#include "SinricProPowerSensor.h"

#define WIFI_SSID         "YOUR-WIFI-SSID"    
#define WIFI_PASS         "YOUR-WIFI-PASSWORD"
#define APP_KEY           "YOUR-APP-KEY"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "YOUR-APP-SECRET"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define TEMP_SENSOR_ID    "YOUR-DEVICE-ID"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                // Change baudrate to your need (used for serial monitor)
#define SAMPLE_EVERY_SEC  60                  // Send every 60 seconds new data to server

bool powerState = true;

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  powerState = state;
  return true; // request handled properly
}

bool sendPowerSensorData() {
  if (!powerState) return false; // just send data if device is turned on

  // limit data rate to SAMPLE_EVERY_SEC
  static unsigned long lastEvent = -(SAMPLE_EVERY_SEC * 1000);
  unsigned long actualMillis = millis();
  if (actualMillis - lastEvent < (SAMPLE_EVERY_SEC * 1000)) return false;
  lastEvent = actualMillis;

  // use some random data
  float voltage = random(2200,2300) / 10.0f;
  float current = random(1,20) / 10.0f;
  float power = voltage * current;
  float apparentPower = power - (random(10,20)/10.0f);

  // get sensor device and send data
  SinricProPowerSensor &myPowerSensor = SinricPro[POWERSENSOR_ID];
  return myPowerSensor.sendPowerSensorEvent(voltage, current, power, apparentPower);
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
  SinricProPowerSensor &myPowerSensor = SinricPro[POWERSENSOR_ID];

  // set callback function to device
  myPowerSensor.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET, "testws.sinric.pro");
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE);
  setupWiFi();
  setupSinricPro();
}



void loop() {
  SinricPro.handle();
  sendPowerSensorData();
}