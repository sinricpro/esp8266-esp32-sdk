#ifndef _THERMOSTATCONTROLLER_H_
#define _THERMOSTATCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

/**
 * @class: ThermostatController
 * @brief: support for onThermostatMode, onTargetTemperature, adjustTargetTemperature, sendThermostatModeEvent, sendTargetTemepratureEvent
 **/ 
class ThermostatController {
  public:
    ThermostatController(SinricProDeviceInterface *device);
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
    using ThermostatModeCallback = std::function<bool(const String &, String &)>;

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
    using SetTargetTemperatureCallback = std::function<bool(const String &, float &)>;

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
    using AdjustTargetTemperatureCallback = std::function<bool(const String &, float &)>;

    void onThermostatMode(ThermostatModeCallback cb);
    void onTargetTemperature(SetTargetTemperatureCallback cb);
    void onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb);

    bool sendThermostatModeEvent(String thermostatMode, String cause = "PHYSICAL_INTERACTION");
    bool sendTargetTemperatureEvent(float temperature, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
    SinricProDeviceInterface* device;
    ThermostatModeCallback thermostatModeCallback;
    SetTargetTemperatureCallback targetTemperatureCallback;
    AdjustTargetTemperatureCallback adjustTargetTemperatureCallback;
};

ThermostatController::ThermostatController(SinricProDeviceInterface* device) : device(device) {}

/**
 * @brief Set callback function for `setThermostatMode` request
 * 
 * @param cb Function pointer to a `ThermostatModeCallback` function
 * @return void
 * @see ThermostatModeCallback
 **/
void ThermostatController::onThermostatMode(ThermostatModeCallback cb) { 
  thermostatModeCallback = cb; 
}

/**
 * @brief Set callback function for `targetTemperature` request
 * 
 * @param cb Function pointer to a `SetTargetTemperatureCallback` function
 * @return void
 * @see SetTargetTemperatureCallback
 **/
void ThermostatController::onTargetTemperature(SetTargetTemperatureCallback cb)
{
  targetTemperatureCallback = cb;
}

/**
 * @brief Set callback function for `adjustTargetTemperature` request
 * 
 * @param cb Function pointer to a `AdjustTargetTemperatureCallback` function
 * @return void
 * @see AdjustTargetTemperatureCallback
 **/
void ThermostatController::onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb)
{
  adjustTargetTemperatureCallback = cb;
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
bool ThermostatController::sendThermostatModeEvent(String thermostatMode, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("setThermostatMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["thermostatMode"] = thermostatMode;
  return device->sendEvent(eventMessage);
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
bool ThermostatController::sendTargetTemperatureEvent(float temperature, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("targetTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["temperature"] = roundf(temperature * 10) / 10.0;
  return device->sendEvent(eventMessage);
}

bool ThermostatController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (actionString == "targetTemperature" && targetTemperatureCallback) {
    float temperature;
    if (request_value.containsKey("temperature"))  {
      temperature = request_value["temperature"];
    }  else {
      temperature = 1;
    }
    success = targetTemperatureCallback(device->getDeviceId(), temperature);
    response_value["temperature"] = temperature;
    return success;
  }

  if (actionString == "adjustTargetTemperature" && adjustTargetTemperatureCallback) {
    float temperatureDelta = request_value["temperature"];
    success = adjustTargetTemperatureCallback(device->getDeviceId(), temperatureDelta);
    response_value["temperature"] = temperatureDelta;
    return success;
  }

  if (actionString == "setThermostatMode" && thermostatModeCallback) {
    String thermostatMode = request_value["thermostatMode"] | "";
    success = thermostatModeCallback(device->getDeviceId(), thermostatMode);
    response_value["thermostatMode"] = thermostatMode;
    return success;
  }

  return success;
}

#endif