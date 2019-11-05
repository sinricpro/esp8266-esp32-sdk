/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLOCK_H_
#define _SINRICLOCK_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProLock_t :  public SinricProDevice_t {
  public:
	  SinricProLock_t(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(const String, bool&)> LockStateCallback; // void onLockState(const char* deviceId, bool& lockState);
    void onLockState(LockStateCallback cb) { lockStateCallback = cb; }

    // event
    bool sendLockStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    LockStateCallback lockStateCallback;
};
typedef SinricProLock_t& SinricProLock;

SinricProLock_t::SinricProLock_t(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice_t(deviceId, eventWaitTime),
  lockStateCallback(nullptr) {}

bool SinricProLock_t::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  bool success = false;
  String actionString = String(action);

  if (actionString == "setLockState" && lockStateCallback) {
    bool lockState = request_value["state"]=="lock"?true:false;
    success = lockStateCallback(String(deviceId), lockState);
    response_value["state"] = lockState?"LOCKED":"UNLOCKED";
    return success;
  }
  return success;
}

bool SinricProLock_t::sendLockStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setLockState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["state"] = "LOCKED" : event_value["state"] = "UNLOCKED";
  return sendEvent(eventMessage);
}
#endif

