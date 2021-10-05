/*
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
#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProLight.h>

#include <map>

#define WIFI_SSID  "YOUR-WIFI-SSID"
#define WIFI_PASS  "YOUR-WIFI-PASS"
#define APP_KEY    "YOUR-APPKEY"     // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "YOUR-APPSECRET"  // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define LIGHT_ID   "YOUR-DEVICEID"   // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE  9600              // Change baudrate to your need for serial log

#define BLUE_PIN  D5  // PIN for BLUE Mosfet  - change this to your need (D5 = GPIO14 on ESP8266)
#define RED_PIN   D6  // PIN for RED Mosfet   - change this to your need (D6 = GPIO12 on ESP8266)
#define GREEN_PIN D7  // PIN for GREEN Mosfet - change this to your need (D7 = GPIO13 on ESP8266)

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Colortemperature lookup table
using ColorTemperatures = std::map<uint16_t, Color>;
ColorTemperatures colorTemperatures{
    //   {Temperature value, {color r, g, b}}
    {2000, {255, 138, 18}},
    {2200, {255, 147, 44}},
    {2700, {255, 169, 87}},
    {3000, {255, 180, 107}},
    {4000, {255, 209, 163}},
    {5000, {255, 228, 206}},
    {5500, {255, 236, 224}},
    {6000, {255, 243, 239}},
    {6500, {255, 249, 253}},
    {7000, {245, 243, 255}},
    {7500, {235, 238, 255}},
    {9000, {214, 225, 255}}};

struct DeviceState {                                   // Stores current device state with following initial values:
    bool  powerState       = false;                    // initial state is off
    Color color            = colorTemperatures[9000];  // color is set to white (9000k)
    int   colorTemperature = 9000;                     // color temperature is set to 9000k
    int   brightness       = 100;                      // brightness is set to 100
} device_state;

SinricProLight& myLight = SinricPro[LIGHT_ID];  // SinricProLight device

void setStripe() {
    int rValue = map(device_state.color.r * device_state.brightness, 0, 255 * 100, 0, 1023);  // calculate red value and map between 0 and 1023 for analogWrite
    int gValue = map(device_state.color.g * device_state.brightness, 0, 255 * 100, 0, 1023);  // calculate green value and map between 0 and 1023 for analogWrite
    int bValue = map(device_state.color.b * device_state.brightness, 0, 255 * 100, 0, 1023);  // calculate blue value and map between 0 and 1023 for analogWrite

    if (device_state.powerState == false) {  // turn off?
        digitalWrite(RED_PIN, LOW);          // set
        digitalWrite(GREEN_PIN, LOW);        // mosfets
        digitalWrite(BLUE_PIN, LOW);         // low
    } else {
        analogWrite(RED_PIN, rValue);    // write red value to pin
        analogWrite(GREEN_PIN, gValue);  // write green value to pin
        analogWrite(BLUE_PIN, bValue);   // write blue value to pin
    }
}

bool onPowerState(const String& deviceId, bool& state) {
    device_state.powerState = state;  // store the new power state
    setStripe();                      // update the mosfets
    return true;
}

bool onBrightness(const String& deviceId, int& brightness) {
    device_state.brightness = brightness;  // store new brightness level
    setStripe();                           // update the mosfets
    return true;
}

bool onAdjustBrightness(const String& deviceId, int& brightnessDelta) {
    device_state.brightness += brightnessDelta;  // calculate and store new absolute brightness
    brightnessDelta = device_state.brightness;   // return absolute brightness
    setStripe();                                 // update the mosfets
    return true;
}

bool onColor(const String& deviceId, byte& r, byte& g, byte& b) {
    device_state.color.r = r;  // store new red value
    device_state.color.g = g;  // store new green value
    device_state.color.b = b;  // store new blue value
    setStripe();               // update the mosfets
    return true;
}

bool onColorTemperature(const String& deviceId, int& colorTemperature) {
    device_state.color            = colorTemperatures[colorTemperature];  // set rgb values from corresponding colortemperauture
    device_state.colorTemperature = colorTemperature;                     // store the current color temperature
    setStripe();                                                          // update the mosfets
    return true;
}

bool onIncreaseColorTemperature(const String& devceId, int& colorTemperature) {
    auto current = colorTemperatures.find(device_state.colorTemperature);  // get current entry from colorTemperature map
    auto next    = std::next(current);                                     // get next element
    if (next == colorTemperatures.end()) next = current;                   // prevent past last element
    device_state.color            = next->second;                          // set color
    device_state.colorTemperature = next->first;                           // set colorTemperature
    colorTemperature              = device_state.colorTemperature;         // return new colorTemperature
    setStripe();
    return true;
}

bool onDecreaseColorTemperature(const String& devceId, int& colorTemperature) {
    auto current = colorTemperatures.find(device_state.colorTemperature);  // get current entry from colorTemperature map
    auto next    = std::prev(current);                                     // get previous element
    if (next == colorTemperatures.end()) next = current;                   // prevent before first element
    device_state.color            = next->second;                          // set color
    device_state.colorTemperature = next->first;                           // set colorTemperature
    colorTemperature              = device_state.colorTemperature;         // return new colorTemperature
    setStripe();
    return true;
}

void setupWiFi() {
    Serial.printf("WiFi: connecting");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.printf(".");
        delay(250);
    }
    Serial.printf("connected\r\nIP is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
    myLight.onPowerState(onPowerState);                              // assign onPowerState callback
    myLight.onBrightness(onBrightness);                              // assign onBrightness callback
    myLight.onAdjustBrightness(onAdjustBrightness);                  // assign onAdjustBrightness callback
    myLight.onColor(onColor);                                        // assign onColor callback
    myLight.onColorTemperature(onColorTemperature);                  // assign onColorTemperature callback
    myLight.onDecreaseColorTemperature(onDecreaseColorTemperature);  // assign onDecreaseColorTemperature callback
    myLight.onIncreaseColorTemperature(onIncreaseColorTemperature);  // assign onIncreaseColorTemperature callback

    SinricPro.begin(APP_KEY, APP_SECRET);  // start SinricPro
}

void setup() {
    Serial.begin(BAUD_RATE);  // setup serial

    pinMode(RED_PIN, OUTPUT);    // set red-mosfet pin as output
    pinMode(GREEN_PIN, OUTPUT);  // set green-mosfet pin as output
    pinMode(BLUE_PIN, OUTPUT);   // set blue-mosfet pin as output

    setupWiFi();       // connect wifi
    setupSinricPro();  // setup SinricPro
}

void loop() {
    SinricPro.handle();  // handle SinricPro communication
}