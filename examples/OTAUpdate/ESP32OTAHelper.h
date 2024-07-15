#if defined(ESP32)

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include "Cert.h" 

OTAResult startOTAUpdate(const String &url) {
  OTAResult result = { false, "" };
  WiFiClientSecure *client = new WiFiClientSecure;

  if (client) {
    client->setCACert(rootCACertificate);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;

      Serial.print("[startOTAUpdate()] begin...\n");
      if (https.begin(*client, url)) {
        Serial.print("[startOTAUpdate()] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been sent and Server response header has been handled
          Serial.printf("[startOTAUpdate()] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            // get length of document (is -1 when Server sends no Content-Length header)
            int contentLength = https.getSize();
            Serial.printf("Content-Length: %d\n", contentLength);

            if (contentLength > 0) {
              bool canBegin = Update.begin(contentLength);
              if (canBegin) {
                WiFiClient *stream = https.getStreamPtr();
                size_t written = Update.writeStream(*stream);

                if (written == contentLength) {
                  Serial.println("[startOTAUpdate()] Written : " + String(written) + " successfully");
                } else {
                  result.errorMessage = "Written only : " + String(written) + "/" + String(contentLength) + ". Retry?";
                  Serial.println("[startOTAUpdate()] " + result.errorMessage);
                }

                if (Update.end()) {
                  Serial.println("[startOTAUpdate()] OTA done!");
                  if (Update.isFinished()) {
                    Serial.println("[startOTAUpdate()] Update successfully completed. Rebooting.");
                    ESP.restart();
                    result.success = true;
                    return result;
                  } else {
                    result.errorMessage = "Update not finished? Something went wrong!";
                    Serial.println("[startOTAUpdate()] " + result.errorMessage);
                  }
                } else {
                  result.errorMessage = "Error Occurred. Error #: " + String(Update.getError());
                  Serial.println("[startOTAUpdate()] " + result.errorMessage);
                }
              } else {
                result.errorMessage = "Not enough space to begin OTA";
                Serial.println("[startOTAUpdate()] " + result.errorMessage);
              }
            } else {
              result.errorMessage = "There was no content length in the response";
              Serial.println("[startOTAUpdate()] " + result.errorMessage);
            }
          } else {
            result.errorMessage = "HTTP response code: " + String(httpCode);
            Serial.println("[startOTAUpdate()] " + result.errorMessage);
          }
        } else {
          result.errorMessage = "GET... failed, error: " + https.errorToString(httpCode);
          Serial.println("[startOTAUpdate()] " + result.errorMessage);
        }

        https.end();
      } else {
        result.errorMessage = "Unable to connect";
        Serial.println("[startOTAUpdate()] " + result.errorMessage);
      }
    }
    delete client;
  } else {
    result.errorMessage = "Unable to create client";
    Serial.println("[startOTAUpdate()] " + result.errorMessage);
  }

  return result;
}

#endif