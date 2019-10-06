#ifndef _SINRICTEMPERATURESENSOR_H_
#define _SINRICTEMPERATURESENSOR_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProTemperaturesensor :  public SinricProDevice {
  public:
	  SinricProTemperaturesensor(const char* deviceId, unsigned long eventWaitTime=30000);
    // callback
	  typedef std::function<bool(String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }

    // event
    void sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    void sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
};

SinricProTemperaturesensor::SinricProTemperaturesensor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr) {}

bool SinricProTemperaturesensor::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
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

void SinricProTemperaturesensor::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  sendEvent(eventMessage);
}

void SinricProTemperaturesensor::sendTemperatureEvent(float temperature, float humidity, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "currentTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["humidity"] = humidity;
  event_value["temperature"] = temperature;
  sendEvent(eventMessage);
}

#endif

