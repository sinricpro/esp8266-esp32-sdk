#include "MySwitchButton.h"

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASS"

#define APP_KEY    "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define APP_SECRET "yyyyyyyy-yyyy-yyyy-yyyy-yyyyyyyyyyyy-yyyyyyyy-yyyy-yyyy-yyyy-yyyyyyyyyyyy"

MySwitchButton relay1("YOUR_DEVICE_ID", 13, 2);

void setupWiFi() {
    Serial.printf("[WiFi]: Connecting");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(250);
    }
    Serial.printf("connected to %s\r\n", WIFI_SSID);
    Serial.printf("[WIFi] IP: %s\r\n", WiFi.localIP().toString().c_str());
    WiFi.setAutoReconnect(true);
}

void setupSinric() {
    SinricPro.onConnected([]() { Serial.printf("[SinricPro]: connected\r\n"); });
    SinricPro.onDisconnected([]() { Serial.printf("[SinricPro]: disconnected\r\n"); });
    SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(false);
    setupWiFi();
    setupSinric();
}

void loop() {
    SinricPro.handle();
}