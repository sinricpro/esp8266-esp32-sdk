/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDIMSWITCH_H_
#define _SINRICDIMSWITCH_H_

#include "SinricProDevice.h"

class SinricProDimSwitch :  public SinricProDevice {
  public:
	  SinricProDimSwitch(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
    typedef std::function<bool(const String&, int&)> PowerLevelCallback; // void onPowerLevel(const char* deviceId, int& powerLevel);
  
    void onPowerLevel(PowerLevelCallback cb) { powerLevelCallback = cb; }
    void onAdjustPowerLevel(PowerLevelCallback cb) { adjustPowerLevelCallback = cb; }

    // event
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    bool sendPowerLevelEvent(int powerLevel, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerLevelCallback powerLevelCallback;
    PowerLevelCallback adjustPowerLevelCallback;
};

SinricProDimSwitch::SinricProDimSwitch(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerLevelCallback(nullptr),
  adjustPowerLevelCallback(nullptr) {}

bool SinricProDimSwitch::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

  if (powerLevelCallback && actionString == "setPowerLevel") {
    int powerLevel = request_value["powerLevel"];
    success = powerLevelCallback(String(deviceId), powerLevel);
    response_value["powerLevel"] = powerLevel;
  }

  if (adjustPowerLevelCallback && actionString == "adjustPowerLevel") {
    int powerLevelDelta = request_value["powerLevelDelta"];
    success = adjustPowerLevelCallback(String(deviceId), powerLevelDelta);
    response_value["powerLevel"] = powerLevelDelta;
  }
  return success;
}

bool SinricProDimSwitch::sendPowerLevelEvent(int powerLevel, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerLevel", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["powerLevel"] = powerLevel;
  return sendEvent(eventMessage);
}

#endif

