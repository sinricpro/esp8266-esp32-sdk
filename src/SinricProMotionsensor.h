/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICMOTIONSENSOR_H_
#define _SINRICMOTIONSENSOR_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProMotionsensor :  public SinricProDevice {
  public:
	  SinricProMotionsensor(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(const String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }

    // event
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    bool sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
};

SinricProMotionsensor::SinricProMotionsensor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr) {}

bool SinricProMotionsensor::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
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

bool SinricProMotionsensor::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  return sendEvent(eventMessage);
}

bool SinricProMotionsensor::sendMotionEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "motion", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"detected":"notDetected";
  return sendEvent(eventMessage);
}

#endif

