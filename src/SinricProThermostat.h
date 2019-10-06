#ifndef _SINRICTHERMOSTAT_H_
#define _SINRICTHERMOSTAT_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProThermostat :  public SinricProDevice {
  public:
	  SinricProThermostat(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    typedef std::function<bool(String, float&)> TargetTemperatureCallback;
    typedef std::function<bool(String, String&)> ThermostatModeCallback;

    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }
    void onTargetTemperatue(TargetTemperatureCallback cb) { targetTemperatureCallback = cb; }
    void onThermostatMode(ThermostatModeCallback cb) { thermostatModeCallback = cb; }

    // event
    void sendPowerStateEvent(bool state, const char* cause = "PHYSICAL_INTERACTION");
    void sendTemperatureEvent(float temperature, float humidity = -1, const char* cause = "PERIODIC_POLL");
    void sendTargetTemperatureEvent(float temperature, const char* cause = "PHYSICAL_INTERACTION");
    void sendThermostatModeEvent(String thermostatMode, const char* cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
    TargetTemperatureCallback targetTemperatureCallback;
    ThermostatModeCallback thermostatModeCallback;
};

SinricProThermostat::SinricProThermostat(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr),
  targetTemperatureCallback(nullptr),
  thermostatModeCallback(nullptr) {}

bool SinricProThermostat::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(String(deviceId), powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }

  if (actionString == "targetTemperature" && targetTemperatureCallback) {
    float temperature = request_value["temperature"] | -1;
    success = targetTemperatureCallback(String(deviceId), temperature);
    response_value["temperature"] = temperature;
    return success;
  }

  if (actionString == "setThermostatMode" && thermostatModeCallback) {
    String thermostatMode = request_value["thermostatMode"] | "";
    success = thermostatModeCallback(String(deviceId), thermostatMode);
    response_value["thermostatMode"] = thermostatMode;
    return success;
  }

  return success;
}

void SinricProThermostat::sendPowerStateEvent(bool state, const char* cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause);
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  sendEvent(eventMessage);
}


void SinricProThermostat::sendTemperatureEvent(float temperature, float humidity, const char* cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "currentTemperature", cause);
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["humidity"] = humidity;
  event_value["temperature"] = temperature;
  sendEvent(eventMessage);
}

void SinricProThermostat::sendTargetTemperatureEvent(float temperature, const char* cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "targetTemperature", cause);
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["temperature"] = temperature;
  sendEvent(eventMessage);
}

void SinricProThermostat::sendThermostatModeEvent(String thermostatMode, const char* cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setThermostatMode", cause);
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["thermostatMode"] = thermostatMode;
  sendEvent(eventMessage);
}


#endif

