/*
 * Example for SinricPro Contactsensor device:
 * - Setup contactsensor device
 * - Support onPowerState to turn on / turn off contactsensor
 * - Checks a contact sensor connected to CONTACT_PIN and send event if state changed
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
#endif

#include "SinricPro.h"
#include "SinricProContactsensor.h"

#define WIFI_SSID         "YOUR-WIFI-SSID"
#define WIFI_PASS         "YOUR-WIFI-PASSWORD"
#define APP_KEY           "YOUR-APP-KEY"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "YOUR-APP-SECRET"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define CONTACT_ID        "YOUR-DEVICE-ID"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         115200                // Change baudrate to your need

#define CONTACT_PIN       5                   // PIN where contactsensor is connected to
                                              // LOW  = contact is open
                                              // HIGH = contact is closed

bool lastContactState = false;
unsigned long lastChange = 0;

/**
 * @brief Checks contactsensor connected to CONTACT_PIN
 *
 * If contactsensor state has changed, send event to SinricPro Server
 * state from digitalRead():
 *      HIGH = contactsensor is closed
 *      LOW  = contactsensor is open
 */
void handleContactsensor() {
  unsigned long actualMillis = millis();
  if (actualMillis - lastChange < 250) return;          // debounce contact state transitions (same as debouncing a pushbutton)

  bool actualContactState = digitalRead(CONTACT_PIN);   // read actual state of contactsensor

  if (actualContactState != lastContactState) {         // if state has changed
    Serial.printf("Contactsensor is %s now\r\n", actualContactState?"open":"closed");
    lastContactState = actualContactState;              // update last known state
    lastChange = actualMillis;                          // update debounce time
    SinricProContactsensor &myContact = SinricPro[CONTACT_ID]; // get contact sensor device
    bool success = myContact.sendContactEvent(actualContactState);      // send event with actual state
    if(!success) {
      Serial.printf("Something went wrong...could not send Event to server!\r\n");
    }
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
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProContactsensor& myContact = SinricPro[CONTACT_ID];

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");

  pinMode(CONTACT_PIN, INPUT);

  setupWiFi();
  setupSinricPro();
}

void loop() {
  handleContactsensor();
  SinricPro.handle();
}
