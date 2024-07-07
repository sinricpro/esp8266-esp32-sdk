#if defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)

#include <Arduino.h>
#include <time.h>

#if defined(ARDUINO_ARCH_RP2040)
  #include <HTTPClient.h>
  #include <HTTPUpdate.h>
  #include <WiFi.h>

  #define OTA_CLASS httpUpdate

#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
  #include "ESP8266httpUpdate.h"

  #define OTA_CLASS ESPhttpUpdate
#endif
 
// Function to perform the OTA update
bool startOTAUpdate(const String& url) {
  Serial.println("Starting OTA update");
 
  #if defined(ARDUINO_ARCH_RP2040)
    WiFiClientSecure client;    
  #elif defined(ESP8266)
     BearSSL::WiFiClientSecure client;     
  #endif
 
  client.setInsecure();
  client.setBufferSizes(4096, 4096); // For OTA to work on limited RAM

  // The line below is optional. It can be used to blink the LED on the board during flashing
  // The LED will be on during download of one buffer of data from the network. The LED will
  // be off during writing that buffer to flash
  // On a good connection the LED should flash regularly. On a bad connection the LED will be
  // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
  // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
  //ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  auto http_ret = OTA_CLASS.update(client, url);  

  //if success reboot 

  switch (http_ret) {
    case HTTP_UPDATE_OK:
      Serial.printf("HTTP_UPDATE_OK\n");
      break;

    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", OTA_CLASS.getLastError(), OTA_CLASS.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
  }

  return false;
}

#endif