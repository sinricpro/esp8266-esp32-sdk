/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLIGHT_H_
#define _SINRICLIGHT_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProLight_t :  public SinricProDevice_t {
  public:
    SinricProLight_t(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
    typedef std::function<bool(const String, bool&)> PowerStateCallback;
    typedef std::function<bool(const String, int&)> BrightnessCallback;
    typedef std::function<bool(const String, byte&, byte&, byte&)> ColorCallback;
    typedef std::function<bool(const String, int&)> ColorTemperatureCallback;
  
    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }
    void onBrightness(BrightnessCallback cb) { brightnessCallback = cb; }
    void onAdjustBrightness(BrightnessCallback cb) { adjustBrightnessCallback = cb; }
    void onColor(ColorCallback cb) { colorCallback = cb; }
    void onColorTemperature(ColorTemperatureCallback cb) { colorTemperatureCallback = cb; }
    void onIncreaseColorTemperature(ColorTemperatureCallback cb) { increaseColorTemperatureCallback = cb; }
    void onDecreaseColorTemperature(ColorTemperatureCallback cb) { decreaseColorTemperatureCallback = cb; }

    // event
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    bool sendBrightnessEvent(int brightness, String cause = "PHYSICAL_INTERACTION");
    bool sendColorEvent(byte r, byte g, byte b, String cause = "PHYSICAL_INTERACTION");
    bool sendColorTemperatureEvent(int colorTemperature, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
    BrightnessCallback brightnessCallback;
    BrightnessCallback adjustBrightnessCallback;
    ColorCallback colorCallback;
    ColorTemperatureCallback colorTemperatureCallback;
    ColorTemperatureCallback increaseColorTemperatureCallback;
    ColorTemperatureCallback decreaseColorTemperatureCallback;
};
typedef SinricProLight_t& SinricProLight;

SinricProLight_t::SinricProLight_t(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice_t(deviceId, eventWaitTime),
  powerStateCallback(nullptr),
  brightnessCallback(nullptr),
  adjustBrightnessCallback(nullptr),
  colorCallback(nullptr),
  colorTemperatureCallback(nullptr),
  increaseColorTemperatureCallback(nullptr),
  decreaseColorTemperatureCallback(nullptr) {}

bool SinricProLight_t::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(String(deviceId), powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }

  if (brightnessCallback && actionString == "setBrightness") {
    int brightness = request_value["brightness"];
    success = brightnessCallback(String(deviceId), brightness);
    response_value["brightness"] = brightness;
  }

  if (adjustBrightnessCallback && actionString == "adjustBrightness") {
    int brightnessDelta = request_value["brightnessDelta"];
    success = adjustBrightnessCallback(String(deviceId), brightnessDelta);
    response_value["brightness"] = brightnessDelta;
  }

  if (colorCallback && actionString == "setColor") {
      unsigned char r, g, b;
      r = request_value["color"]["r"];
      g = request_value["color"]["g"];
      b = request_value["color"]["b"];
      success = colorCallback(String(deviceId), r, g, b);
      response_value.createNestedObject("color");
      response_value["color"]["r"] = r;
      response_value["color"]["g"] = g;
      response_value["color"]["b"] = b;
  }

  if (colorTemperatureCallback && actionString == "setColorTemperature") {
    int colorTemperature = request_value["colorTemperature"];
    success = colorTemperatureCallback(String(deviceId), colorTemperature);
    response_value["colorTemperature"] = colorTemperature;
  }

  if (increaseColorTemperatureCallback && actionString == "increaseColorTemperature") {
    int colorTemperature = 1;
    success = increaseColorTemperatureCallback(String(deviceId), colorTemperature);
    response_value["colorTemperature"] = colorTemperature;
  }

  if (decreaseColorTemperatureCallback && actionString == "decreaseColorTemperature") {
    int colorTemperature = -1;
    success = decreaseColorTemperatureCallback(String(deviceId), colorTemperature);
    response_value["colorTemperature"] = colorTemperature;
  }

  return success;
}

bool SinricProLight_t::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  return sendEvent(eventMessage);
}

bool SinricProLight_t::sendBrightnessEvent(int brightness, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setBrightness", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["brightness"] = brightness;
  return sendEvent(eventMessage);
}

bool SinricProLight_t::sendColorEvent(byte r, byte g, byte b, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setColor", cause.c_str());
  JsonObject event_color = eventMessage["payload"]["value"].createNestedObject("color");
  event_color["r"] = r;
  event_color["g"] = g;
  event_color["b"] = b;
  return sendEvent(eventMessage);
}

bool SinricProLight_t::sendColorTemperatureEvent(int colorTemperature, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setColorTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["colorTemperature"] = colorTemperature;
  return sendEvent(eventMessage);
}


#endif

