#if defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)

#include <Arduino.h>
#include <time.h>
#include "Cert.h" 

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

String extractOTAHostname(const String& url) {
  int index = url.indexOf("//") + 2;
  if (index < 0) {
    return "";  // Handle invalid URL format
  }

  int endIndex = url.indexOf("/", index);
  if (endIndex < 0) {
    endIndex = url.length();
  }

  return url.substring(index, endIndex);
}

// Function to perform the OTA update
String startOtaUpdate(const String& url) {  
  #if defined(ARDUINO_ARCH_RP2040)
    WiFiClientSecure client;    
    client.setBufferSizes(4096, 4096); // For OTA to work on limited RAM
  #elif defined(ESP8266)
    BearSSL::WiFiClientSecure client;     
    // Use MFLN to reduce the memory usage 
    String host = extractOTAHostname(url);
    bool mfln = client.probeMaxFragmentLength(host, 443, 512);
    Serial.printf("[startOtaUpdate()] MFLN supported: %s\n", mfln ? "yes" : "no");
    if (mfln) { client.setBufferSizes(512, 512); } else client.setBufferSizes(4096, 4096);
  #endif
 
  client.setInsecure();  

  // The line below is optional. It can be used to blink the LED on the board during flashing
  // The LED will be on during download of one buffer of data from the network. The LED will
  // be off during writing that buffer to flash
  // On a good connection the LED should flash regularly. On a bad connection the LED will be
  // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
  // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
  //ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  Serial.printf("[startOtaUpdate()] Starting the OTA update. This may take few mins to complete!\n");
  auto http_ret = OTA_CLASS.update(client, url);  

  //if success reboot will reboot! 
  String errorMsg = "";

  switch (http_ret) {
    case HTTP_UPDATE_OK:
      Serial.printf("[startOtaUpdate()] HTTP_UPDATE_OK\n");
      break;

    case HTTP_UPDATE_FAILED:
      errorMsg = String("HTTP_UPDATE_FAILED Error (") + OTA_CLASS.getLastError() + "): " + OTA_CLASS.getLastErrorString();
      Serial.printf("%s\n", errorMsg.c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      errorMsg = "HTTP_UPDATE_NO_UPDATES";
      Serial.println(errorMsg);
      break;
  }
  
  return errorMsg;
}

#endif