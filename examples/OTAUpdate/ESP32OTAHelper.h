#if defined(ESP32)

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include "Cert.h"

// Function to perform the OTA update
bool startOTAUpdate(const String& url) {
  WiFiClientSecure *client = new WiFiClientSecure;
  
  if(client) {
    client->setCACert(rootCACertificate);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https; 
    
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, url)) {
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            // get length of document (is -1 when Server sends no Content-Length header)
            int contentLength = https.getSize();
            Serial.printf("Content-Length: %d\n", contentLength);
  
            if (contentLength > 0) {
              bool canBegin = Update.begin(contentLength);
              if (canBegin) {
                WiFiClient * stream = https.getStreamPtr();
                size_t written = Update.writeStream(*stream);
  
                if (written == contentLength) {
                  Serial.println("Written : " + String(written) + " successfully");
                } else {
                  Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                }
  
                if (Update.end()) {
                  Serial.println("OTA done!");
                  if (Update.isFinished()) {
                    Serial.println("Update successfully completed. Rebooting.");
                    ESP.restart();
                    return true;
                  } else {
                    Serial.println("Update not finished? Something went wrong!");
                  }
                } else {
                  Serial.println("Error Occurred. Error #: " + String(Update.getError()));
                }
              } else {
                Serial.println("Not enough space to begin OTA");
              }
            } else {
              Serial.println("There was no content length in the response");
            }
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  
  return false;
} 

#endif