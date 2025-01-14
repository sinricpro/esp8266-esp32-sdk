#if defined(ESP32)

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

String startOtaUpdate(const String &url) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    Serial.print("[startOtaUpdate()] begin...\n");
    if (!https.begin(client, url)) return "Unable to connect";

    Serial.print("[startOtaUpdate()] GET...\n");
    // start connection and send HTTP header
    int httpCode = https.GET();
    if (httpCode != HTTP_CODE_OK) return "GET... failed, error: " + https.errorToString(httpCode);
    
    int contentLength = https.getSize();
    Serial.printf("OTA size: %d bytes \n", contentLength);

    if (contentLength == 0) return "There was no content length in the response";

    Serial.printf("Beginning update..!\n");
    bool canBegin = Update.begin(contentLength);

    if (!canBegin) return "Not enough space to begin OTA";

    WiFiClient *stream  = https.getStreamPtr();
    size_t      written = Update.writeStream(*stream);

    if (written != contentLength) return "Written only : " + String(written) + "/" + String(contentLength) + ". Retry?";
    Serial.println("[startOtaUpdate()] Written : " + String(written) + " successfully");
    
    if (!Update.end()) return "Error Occurred. Error #: " + String(Update.getError());
    Serial.println("[startOtaUpdate()] OTA done!");

    if (!Update.isFinished()) return "Update not finished? Something went wrong!";
    Serial.println("[startOtaUpdate()] Update successfully completed. Rebooting.");
    ESP.restart();

    return "";
}

#endif
