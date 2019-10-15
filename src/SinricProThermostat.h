/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTHERMOSTAT_H_
#define _SINRICTHERMOSTAT_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProThermostat :  public SinricProDevice {
  public:
	  SinricProThermostat(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(const String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    typedef std::function<bool(const String, float&)> TargetTemperatureCallback;
    typedef std::function<bool(const String, String&)> ThermostatModeCallback;

    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }
    void onTargetTemperatue(TargetTemperatureCallback cb) { targetTemperatureCallback = cb; }
    void onAdjustTargetTemperature(TargetTemperatureCallback cb) { adjustTargetTemperatureCallback = cb; }
    void onThermostatMode(ThermostatModeCallback cb) { thermostatModeCallback = cb; }

    // event
    void sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    void sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL");
    void sendTargetTemperatureEvent(float temperature, String cause = "PHYSICAL_INTERACTION");
    void sendThermostatModeEvent(String thermostatMode, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
    TargetTemperatureCallback targetTemperatureCallback;
    TargetTemperatureCallback adjustTargetTemperatureCallback;
    ThermostatModeCallback thermostatModeCallback;
};

SinricProThermostat::SinricProThermostat(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr),
  targetTemperatureCallback(nullptr),
  adjustTargetTemperatureCallback(nullptr),
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
    float temperature;
    if (request_value.containsKey("temperature")) {
      temperature = request_value["temperature"];
    } else {
      temperature = 1;
    }
    success = targetTemperatureCallback(String(deviceId), temperature);
    response_value["temperature"] = temperature;
    return success;
  }

  if (actionString == "adjustTemperature" && adjustTargetTemperatureCallback) {
    float temperatureDelta = request_value["temperature"];
    success = adjustTargetTemperatureCallback(String(deviceId), temperatureDelta);
    response_value["temperature"] = temperatureDelta;
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

void SinricProThermostat::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  sendEvent(eventMessage);
}


void SinricProThermostat::sendTemperatureEvent(float temperature, float humidity, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "currentTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["humidity"] = humidity;
  event_value["temperature"] = temperature;
  sendEvent(eventMessage);
}

void SinricProThermostat::sendTargetTemperatureEvent(float temperature, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "targetTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["temperature"] = temperature;
  sendEvent(eventMessage);
}

void SinricProThermostat::sendThermostatModeEvent(String thermostatMode, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setThermostatMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["thermostatMode"] = thermostatMode;
  sendEvent(eventMessage);
}


#endif

