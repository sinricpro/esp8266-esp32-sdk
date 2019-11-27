/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLOCK_H_
#define _SINRICLOCK_H_

#include "SinricProDevice.h"

class SinricProLock :  public SinricProDevice {
  public:
	  SinricProLock(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(const String&, bool&)> LockStateCallback; // void onLockState(const char* deviceId, bool& lockState);
    void onLockState(LockStateCallback cb) { lockStateCallback = cb; }
    void onPowerState() = delete;  // SinricProLock has no powerState
    // event
    bool sendPowerStateEvent() = delete; // SinricProLock has no powerState
    bool sendLockStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    LockStateCallback lockStateCallback;
};

SinricProLock::SinricProLock(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  lockStateCallback(nullptr) {}

bool SinricProLock::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  bool success = false;
  String actionString = String(action);

  if (actionString == "setLockState" && lockStateCallback) {
    bool lockState = request_value["state"]=="lock"?true:false;
    success = lockStateCallback(String(deviceId), lockState);
    response_value["state"] = success?lockState?"LOCKED":"UNLOCKED":"JAMMED";
    return success;
  }
  return success;
}

bool SinricProLock::sendLockStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setLockState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["state"] = "LOCKED" : event_value["state"] = "UNLOCKED";
  return sendEvent(eventMessage);
}
#endif

