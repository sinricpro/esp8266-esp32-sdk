/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTEMPERATURESENSOR_H_
#define _SINRICTEMPERATURESENSOR_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProTemperaturesensor_t :  public SinricProDevice_t {
  public:
	  SinricProTemperaturesensor_t(const char* deviceId, unsigned long eventWaitTime=30000);
    // callback
	  typedef std::function<bool(const String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }

    // event
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    bool sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
};
typedef SinricProTemperaturesensor_t& SinricProTemperaturesensor;

SinricProTemperaturesensor_t::SinricProTemperaturesensor_t(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice_t(deviceId, eventWaitTime),
  powerStateCallback(nullptr) {}

bool SinricProTemperaturesensor_t::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(String(deviceId), powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }
  return success;
}

bool SinricProTemperaturesensor_t::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  return sendEvent(eventMessage);
}

bool SinricProTemperaturesensor_t::sendTemperatureEvent(float temperature, float humidity, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "currentTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["humidity"] = roundf(humidity * 10) / 10.0;
  event_value["temperature"] = roundf(temperature * 10) / 10.0;
  return sendEvent(eventMessage);
}

#endif

