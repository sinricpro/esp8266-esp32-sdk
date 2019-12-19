/*
 * Example for SinricPro Motionsensor device:
 * - Setup motionsensor device
 * - Support onPowerState to turn on / turn off motion sensor
 * - Checks a motion sensor connected to MOTIONSENSOR_PIN and send event if state changed
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
#include "SinricProMotionsensor.h"

#define WIFI_SSID         "YOUR-WIFI-SSID"
#define WIFI_PASS         "YOUR-WIFI-PASSWORD"
#define APP_KEY           "YOUR-APP-KEY"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "YOUR-APP-SECRET"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define MOTIONSENSOR_ID   "YOUR-DEVICE-ID"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                // Change baudrate to your need

#define MOTIONSENSOR_PIN  5                   // PIN where motionsensor is connected to
                                              // LOW  = motion is not detected
                                              // HIGH = motion is detected


bool myPowerState = true;                     // assume device is turned on
bool lastMotionState = false;
unsigned long lastChange = 0;

/**
 * @brief Checks motionsensor connected to MOTIONSENSOR_PIN
 *
 * If motion sensor state has changed, send event to SinricPro Server
 * state from digitalRead():
 *      HIGH = motion detected
 *      LOW  = motion not detected
 */
void handleMotionsensor() {
  if (!myPowerState) return;                            // if device switched off...do nothing

  unsigned long actualMillis = millis();
  if (actualMillis - lastChange < 250) return;          // debounce motionsensor state transitions (same as debouncing a pushbutton)

  bool actualMotionState = digitalRead(MOTIONSENSOR_PIN);   // read actual state of motion sensor

  if (actualMotionState != lastMotionState) {         // if state has changed
    Serial.printf("Motion %s\r\n", actualMotionState?"detected":"not detected");
    lastMotionState = actualMotionState;              // update last known state
    lastChange = actualMillis;                        // update debounce time
    SinricProMotionsensor &myMotionsensor = SinricPro[MOTIONSENSOR_ID]; // get motion sensor device
    myMotionsensor.sendMotionEvent(actualMotionState);
  }
}

/**
 * @brief Callback for setPowerState request
 *
 * @param deviceId      String containing deviceId (useful if this callback used by multiple devices)
 * @param[in] state     bool true=turn on device / false=turn off device
 * @param[out] state    bool true=device turned on / false=device turned off
 * @return true         request handled properly
 * @return false        request can't be handled because some kind of error happened
 */
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  myPowerState = state;
  return true; // request handled properly
}


// setup function for WiFi connection
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

// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProMotionsensor& myMotionsensor = SinricPro[MOTIONSENSOR_ID];

  // set callback function to device
  myMotionsensor.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");

  pinMode(MOTIONSENSOR_PIN, INPUT);

  setupWiFi();
  setupSinricPro();
}

void loop() {
  handleMotionsensor();
  SinricPro.handle();
}
