#ifndef _SINRICCONTACTSENSOR_H_
#define _SINRICCONTACTSENSOR_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProContactsensor :  public SinricProDevice {
  public:
	  SinricProContactsensor(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }

    // event
    void sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    void sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
};

SinricProContactsensor::SinricProContactsensor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr) {}

bool SinricProContactsensor::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
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

void SinricProContactsensor::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  sendEvent(eventMessage);
}

void SinricProContactsensor::sendContactEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setContactState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"closed":"open";
  sendEvent(eventMessage);
}


#endif

