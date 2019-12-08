/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTHERMOSTAT_H_
#define _SINRICTHERMOSTAT_H_

#include "SinricProDevice.h"

/**
 * @class SinricProThermostat
 * @brief Device to control Thermostat
 * 
 * Support
 * * Set / adjust target temperature
 * * Report target temperature
 * * Report actual temperature
 * * Set thermostat mode `AUTO`, `COOL`, `HEAT`
 **/
class SinricProThermostat :  public SinricProDevice {
  public:
	  SinricProThermostat(const char* deviceId, unsigned long eventWaitTime=60000);
    // callback

   /**
     * @brief Callback definition for onTargetTemperature function
     * 
     * Gets called when device receive a `targetTemperature` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   temperature Float device is requested to set the target temperature to
     * @param[out]  temperature Float target temperature temperature device is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section TargetTemperatureCallback Example-Code
     * @snippet callbacks.cpp onTargetTemperature
     **/
    typedef std::function<bool(const String&, float&)> SetTargetTemperatureCallback;

   /**
     * @brief Callback definition for onAdjustTargetTemperature function
     * 
     * Gets called when device receive a `adjustTargetTemperature` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   temperature Float relative temperature device should change about
     * @param[out]  temperature Float absolute temperature device is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustTargetTemperatureCallback Example-Code
     * @snippet callbacks.cpp onAdjustTargetTemperature
     **/
    typedef std::function<bool(const String&, float&)> AdjustTargetTemperatureCallback;

   /**
     * @brief Callback definition for onThermostatMode function
     * 
     * Gets called when device receive a `setThermostatMode` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   mode        String with mode device should set to
     * @param[out]  mode        String device mode is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section ThermostatModeCallback Example-Code
     * @snippet callbacks.cpp onThermostatMode
     **/
    typedef std::function<bool(const String&, String&)> ThermostatModeCallback;

    void onTargetTemperature(SetTargetTemperatureCallback cb);
    void onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb);
    void onThermostatMode(ThermostatModeCallback cb);

    // event
    bool sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL");
    bool sendTargetTemperatureEvent(float temperature, String cause = "PHYSICAL_INTERACTION");
    bool sendThermostatModeEvent(String thermostatMode, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    SetTargetTemperatureCallback targetTemperatureCallback;
    AdjustTargetTemperatureCallback adjustTargetTemperatureCallback;
    ThermostatModeCallback thermostatModeCallback;
};

SinricProThermostat::SinricProThermostat(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  targetTemperatureCallback(nullptr),
  adjustTargetTemperatureCallback(nullptr),
  thermostatModeCallback(nullptr) {}

bool SinricProThermostat::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

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

  if (actionString == "adjustTargetTemperature" && adjustTargetTemperatureCallback) {
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

/**
 * @brief Set callback function for `targetTemperature` request
 * 
 * @param cb Function pointer to a `SetTargetTemperatureCallback` function
 * @return void
 * @see SetTargetTemperatureCallback
 **/
void SinricProThermostat::onTargetTemperature(SetTargetTemperatureCallback cb) { 
  targetTemperatureCallback = cb; 
}

/**
 * @brief Set callback function for `adjustTargetTemperature` request
 * 
 * @param cb Function pointer to a `AdjustTargetTemperatureCallback` function
 * @return void
 * @see AdjustTargetTemperatureCallback
 **/
void SinricProThermostat::onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb) { 
  adjustTargetTemperatureCallback = cb; 
}

/**
 * @brief Set callback function for `setThermostatMode` request
 * 
 * @param cb Function pointer to a `ThermostatModeCallback` function
 * @return void
 * @see ThermostatModeCallback
 **/
void SinricProThermostat::onThermostatMode(ThermostatModeCallback cb) { 
  thermostatModeCallback = cb; 
}

/**
 * @brief Send `currentTemperature` event to report actual temperature (measured by a sensor)
 * 
 * @param   temperature   Float with actual temperature measured by a sensor
 * @param   humidity      (optional) Float with actual humidity measured by a sensor (default=-1.0f means not supported)
 * @param   cause         (optional) `String` reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the even
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProThermostat::sendTemperatureEvent(float temperature, float humidity, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "currentTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["humidity"] = humidity;
  event_value["temperature"] = roundf(temperature *10) / 10;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `targetTemperature` event to report target temperature change
 * 
 * @param   temperature   Float with actual target temperature the device is set to
 * @param   cause         (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProThermostat::sendTargetTemperatureEvent(float temperature, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "targetTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["temperature"] = roundf(temperature * 10) / 10.0;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `thermostatMode` event to report a the new mode the device has been set to
 * 
 * @param   thermostatMode  String with actual mode (`AUTO`, `COOL`, `HEAT`) the device is set to
 * @param   cause           (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true            event has been sent successfully
 * @retval  false           event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProThermostat::sendThermostatModeEvent(String thermostatMode, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setThermostatMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["thermostatMode"] = thermostatMode;
  return sendEvent(eventMessage);
}


#endif

