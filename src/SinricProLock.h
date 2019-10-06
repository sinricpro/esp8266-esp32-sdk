#ifndef _SINRICLOCK_H_
#define _SINRICLOCK_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProLock :  public SinricProDevice {
  public:
	  SinricProLock(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(String, bool&)> LockStateCallback; // void onLockState(const char* deviceId, bool& lockState);
    void onLockState(LockStateCallback cb) { lockStateCallback = cb; }

    // event
    void sendLockStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

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
    response_value["state"] = lockState?"LOCKED":"UNLOCKED";
    return success;
  }
  return success;
}

void SinricProLock::sendLockStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setLockState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["state"] = "LOCKED" : event_value["state"] = "UNLOCKED";
  sendEvent(eventMessage);
}
#endif

