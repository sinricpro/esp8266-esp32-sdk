/*
 * Example for how to use SinricPro Doorbell device:
 * - setup a doorbell device
 * - send event to sinricPro server if button is pressed
 */

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProDoorbell.h"

#define WIFI_SSID         "YOUR-WIFI-SSID"
#define WIFI_PASS         "YOUR-WIFI-PASSWORD"
#define SOCKET_AUTH_TOKEN "YOUR-SOCKET-AUTH-TOKEN"
#define SIGNING_KEY       "YOUR-SIGNING-KEY"

#define DOORBELL_ID       "YOUR-DEVICE-ID"

// change this to your button PIN
// on NodeMCU D3 / GPIO-0 is flash button PIN so you can use the builtin flash button
#define BUTTON_PIN 0


// checkButtonpress
// reads if BUTTON_PIN gets LOW and send Event
void checkButtonPress() {
  static unsigned long lastBtnPress;
  unsigned long actualMillis = millis();

  if (actualMillis-lastBtnPress > 500) {
    if (digitalRead(BUTTON_PIN)==LOW) {
      lastBtnPress = actualMillis;

      // get Doorbell device back
      SinricProDoorbell& myDoorbell = SinricPro[DOORBELL_ID];

      // send doorbell event
      myDoorbell.sendDoorbellEvent();
    }
  }
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
  // add doorbell device to SinricPro
  SinricPro.add<SinricProDoorbell>(DOORBELL_ID);
  // setup SinricPro
  SinricPro.begin(SOCKET_AUTH_TOKEN, SIGNING_KEY);
}

// main setup function
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // BUTTIN_PIN as INPUT

  Serial.begin(115200);
  setupWiFi();
  setupSinricPro();
}

void loop() {
  checkButtonPress();
  SinricPro.handle();
}
