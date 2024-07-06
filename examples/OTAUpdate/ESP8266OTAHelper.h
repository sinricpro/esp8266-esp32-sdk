#if defined(ESP8266)

#include <Arduino.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include "ESP8266httpUpdate.h"
#include <ESP8266WiFi.h>
 
// Function to perform the OTA update
bool startOTAUpdate(const String& url) {
  Serial.println("Starting OTA update");
  Serial.printf("Available RAM: %d\n", ESP.getFreeHeap()); 

  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  client.setBufferSizes(4096, 4096);

  // The line below is optional. It can be used to blink the LED on the board during flashing
  // The LED will be on during download of one buffer of data from the network. The LED will
  // be off during writing that buffer to flash
  // On a good connection the LED should flash regularly. On a bad connection the LED will be
  // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
  // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
  //ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  auto http_ret = ESPhttpUpdate.update(client, url);

  //if success reboot 

  switch (http_ret) {
    case HTTP_UPDATE_OK:
      Serial.printf("HTTP_UPDATE_OK\n");
      break;

    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
  }

  return false;
}

#endif