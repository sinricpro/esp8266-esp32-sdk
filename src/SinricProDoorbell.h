#ifndef _SINRICDOORBELL_H_
#define _SINRICDOORBELL_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProDoorbell :  public SinricProDevice {
  public:
	  SinricProDoorbell(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }

    // event
    void sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    void sendDoorbellEvent(String cause = "PHYSICAL_INTERACTION");
    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
};

SinricProDoorbell::SinricProDoorbell(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr) {}

bool SinricProDoorbell::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
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

void SinricProDoorbell::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  sendEvent(eventMessage);
}

void SinricProDoorbell::sendDoorbellEvent(String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "DoorbellPress", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = "pressed";
  sendEvent(eventMessage);
}

#endif

