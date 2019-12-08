/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICWINDOWAC_H_
#define _SINRICWINDOWAC_H_

#include "SinricProDevice.h"

/**
 * @class SinricProWindowAC
 * @brief Device to control Window Air Conditioner 
 * 
 * Support
 * * Set / adjust target temperature
 * * Set mode `AUTO`, `COOL`, `HEAT`
 * * Set range value 
 * * Report target temperature
 * * Report actual temperature
 **/
class SinricProWindowAC :  public SinricProDevice {
  public:
	  SinricProWindowAC(const char* deviceId, unsigned long eventWaitTime=30000);
    // callback

    /**
     * @brief Callback definition for onRangeValue function
     * 
     * Gets called when device receive a `setRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer 0..n for range value device has to be set
     * @param[out]  rangeValue  Integer 0..n returning the current range value
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SetRangeValueCallback_WAC Example-Code
     * @snippet callbacks.cpp onRangeValue
     **/
    typedef std::function<bool(const String&, int&)> SetRangeValueCallback;

    /**
     * @brief Callback definition for onAdjustRangeValue function
     * 
     * Gets called when device receive a `adjustRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer -n..n delta value for range value have to change
     * @param[out]  rangeValue  Integer 0..n returning the absolute range value 
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustRangeValueCallback_WAC Example-Code
     * @snippet callbacks.cpp onAdjustRangeValue
     **/
    typedef std::function<bool(const String&, int&)> AdjustRangeValueCallback;

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
     * @section TargetTemperatureCallback_WAC Example-Code
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
     * @section AdjustTargetTemperatureCallback_WAC Example-Code
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
     * @section ThermostatModeCallback_WAC Example-Code
     * @snippet callbacks.cpp onThermostatMode
     **/
    typedef std::function<bool(const String&, String&)> ThermostatModeCallback;

    void onRangeValue(SetRangeValueCallback cb);
    void onAdjustRangeValue(AdjustRangeValueCallback cb);
    void onTargetTemperature(SetTargetTemperatureCallback cb);
    void onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb);
    void onThermostatMode(ThermostatModeCallback cb);

    // event
    bool sendRangeValueEvent(int rangeValue, String cause = "PHYSICAL_INTERACTION");
    bool sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL");
    bool sendTargetTemperatureEvent(float temperature, String cause = "PHYSICAL_INTERACTION");
    bool sendThermostatModeEvent(String thermostatMode, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    SetRangeValueCallback rangeValueCallback; 
    AdjustRangeValueCallback adjustRangeValueCallback; 
    SetTargetTemperatureCallback targetTemperatureCallback;
    AdjustTargetTemperatureCallback adjustTargetTemperatureCallback;
    ThermostatModeCallback thermostatModeCallback;

};

SinricProWindowAC::SinricProWindowAC(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  rangeValueCallback(nullptr),
  adjustRangeValueCallback(nullptr),
  targetTemperatureCallback(nullptr),
  adjustTargetTemperatureCallback(nullptr),
  thermostatModeCallback(nullptr) {}

bool SinricProWindowAC::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

  if (actionString == "setRangeValue" && rangeValueCallback) {
    int rangeValue = request_value["rangeValue"] | 0;
    success = rangeValueCallback(String(deviceId), rangeValue);
    response_value["rangeValue"] = rangeValue;
    return success;
  }

  if (actionString == "adjustRangeValue" && adjustRangeValueCallback) {
    int rangeValueDelta = request_value["rangeValueDelta"] | 0;
    success = adjustRangeValueCallback(String(deviceId), rangeValueDelta);
    response_value["rangeValue"] = rangeValueDelta;
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
 * @brief Set callback function for `setRangeValue` request
 * 
 * @param cb Function pointer to a `SetRangeValueCallback` function
 * @see SetRangeValueCallback
 */
void SinricProWindowAC::onRangeValue(SetRangeValueCallback cb) { rangeValueCallback = cb; }

/**
 * @brief Set callback function for `adjustRangeValue` request
 * 
 * @param cb Function pointer to a `AdjustRangeValueCallback` function
 * @see AdjustRangeValueCallback
 */
void SinricProWindowAC::onAdjustRangeValue(AdjustRangeValueCallback cb) { adjustRangeValueCallback = cb; }

/**
 * @brief Set callback function for `targetTemperature` request
 * 
 * @param cb Function pointer to a `SetTargetTemperatureCallback` function
 * @return void
 * @see SetTargetTemperatureCallback
 **/
void SinricProWindowAC::onTargetTemperature(SetTargetTemperatureCallback cb) { targetTemperatureCallback = cb; }

/**
 * @brief Set callback function for `adjustTargetTemperature` request
 * 
 * @param cb Function pointer to a `AdjustTargetTemperatureCallback` function
 * @return void
 * @see AdjustTargetTemperatureCallback
 **/
void SinricProWindowAC::onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb) { adjustTargetTemperatureCallback = cb; }

/**
 * @brief Set callback function for `setThermostatMode` request
 * 
 * @param cb Function pointer to a `ThermostatModeCallback` function
 * @return void
 * @see ThermostatModeCallback
 **/
void SinricProWindowAC::onThermostatMode(ThermostatModeCallback cb) { thermostatModeCallback = cb; }

/**
 * @brief Send `rangeValue` event to report curent rangeValue to SinricPro server
 * 
 * @param   rangeValue  Value between 0..n
 * @param   cause       (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true        event has been sent successfully
 * @retval  false       event has not been sent, maybe you sent to much events in a short distance of time
 */
bool SinricProWindowAC::sendRangeValueEvent(int rangeValue, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setRangeValue", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return sendEvent(eventMessage);
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
bool SinricProWindowAC::sendTemperatureEvent(float temperature, float humidity, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "currentTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["humidity"] = humidity;
  event_value["temperature"] = roundf(temperature * 10)  / 10.0;
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
bool SinricProWindowAC::sendTargetTemperatureEvent(float temperature, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "targetTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["temperature"] = roundf(temperature*10) / 10.0;
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
bool SinricProWindowAC::sendThermostatModeEvent(String thermostatMode, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setThermostatMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["thermostatMode"] = thermostatMode;
  return sendEvent(eventMessage);
}

#endif