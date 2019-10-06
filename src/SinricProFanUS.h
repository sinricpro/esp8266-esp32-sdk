#ifndef _SINRICFANUS_H_
#define _SINRICFANUS_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProFanUS :  public SinricProDevice {
  public:
	  SinricProFanUS(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    typedef std::function<bool(String, int&)> RangeValueCallback;
    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }
    void onRangeValue(RangeValueCallback cb) { rangeValueCallback = cb; }

    // event
    void sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    void sendRangeValueEvent(int rangeValue, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
    RangeValueCallback rangeValueCallback; 
};

SinricProFanUS::SinricProFanUS(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr),
  rangeValueCallback(nullptr) {}

bool SinricProFanUS::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(String(deviceId), powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }

  if (actionString == "setRangeValue" && rangeValueCallback) {
    int rangeValue = request_value["rangeValue"] | 0;
    success = rangeValueCallback(String(deviceId), rangeValue);
    response_value["rangeValue"] = rangeValue;
    return success;
  }
  return success;
}

void SinricProFanUS::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  sendEvent(eventMessage);
}

void SinricProFanUS::sendRangeValueEvent(int rangeValue, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setRangeValue", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  sendEvent(eventMessage);
}

#endif

