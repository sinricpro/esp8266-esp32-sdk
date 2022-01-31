#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(THERMOSTAT, setThermostatMode);           // "setThermostatMode"
FSTR(THERMOSTAT, thermostatMode);              // "thermostatMode"
FSTR(THERMOSTAT, targetTemperature);           // "targetTemperature"
FSTR(THERMOSTAT, temperature);                 // "temperature"
FSTR(THERMOSTAT, adjustTargetTemperature);     // "adjustTargetTemperature"

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


/**
 * @brief ThermostatController
 * @ingroup Capabilities
 **/
template <typename T>
class ThermostatController {
  public:
    ThermostatController();

    void onThermostatMode(ThermostatModeCallback cb);
    void onTargetTemperature(SetTargetTemperatureCallback cb);
    void onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb);

    bool sendThermostatModeEvent(String thermostatMode, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
    bool sendTargetTemperatureEvent(float temperature, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handleThermostatController(SinricProRequest &request);

  private:
    EventLimiter event_limiter_thermostatMode;
    EventLimiter event_limiter_targetTemperature;
    ThermostatModeCallback thermostatModeCallback;
    SetTargetTemperatureCallback targetTemperatureCallback;
    AdjustTargetTemperatureCallback adjustTargetTemperatureCallback;
};

template <typename T>
ThermostatController<T>::ThermostatController()
: event_limiter_thermostatMode(EVENT_LIMIT_STATE)
, event_limiter_targetTemperature(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&ThermostatController<T>::handleThermostatController, this, std::placeholders::_1)); 
}

/**
 * @brief Set callback function for `setThermostatMode` request
 * 
 * @param cb Function pointer to a `ThermostatModeCallback` function
 * @return void
 * @see ThermostatModeCallback
 **/
template <typename T>
void ThermostatController<T>::onThermostatMode(ThermostatModeCallback cb) {
  thermostatModeCallback = cb;
}

/**
 * @brief Set callback function for `targetTemperature` request
 * 
 * @param cb Function pointer to a `SetTargetTemperatureCallback` function
 * @return void
 * @see SetTargetTemperatureCallback
 **/
template <typename T>
void ThermostatController<T>::onTargetTemperature(SetTargetTemperatureCallback cb) {
  targetTemperatureCallback = cb;
}

/**
 * @brief Set callback function for `adjustTargetTemperature` request
 * 
 * @param cb Function pointer to a `AdjustTargetTemperatureCallback` function
 * @return void
 * @see AdjustTargetTemperatureCallback
 **/
template <typename T>
void ThermostatController<T>::onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb) {
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
template <typename T>
bool ThermostatController<T>::sendThermostatModeEvent(String thermostatMode, String cause) {
  if (event_limiter_thermostatMode) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_THERMOSTAT_setThermostatMode, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_THERMOSTAT_thermostatMode] = thermostatMode;
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
template <typename T>
bool ThermostatController<T>::sendTargetTemperatureEvent(float temperature, String cause) {
  if (event_limiter_targetTemperature) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_THERMOSTAT_targetTemperature, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_THERMOSTAT_temperature] = roundf(temperature * 10) / 10.0;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ThermostatController<T>::handleThermostatController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (request.action == FSTR_THERMOSTAT_targetTemperature && targetTemperatureCallback) {
    float temperature;
    if (request.request_value.containsKey(FSTR_THERMOSTAT_temperature))  {
      temperature = request.request_value[FSTR_THERMOSTAT_temperature];
    }  else {
      temperature = 1;
    }
    success = targetTemperatureCallback(device->deviceId, temperature);
    request.response_value[FSTR_THERMOSTAT_temperature] = temperature;
    return success;
  }

  if (request.action == FSTR_THERMOSTAT_adjustTargetTemperature && adjustTargetTemperatureCallback) {
    float temperatureDelta = request.request_value[FSTR_THERMOSTAT_temperature];
    success = adjustTargetTemperatureCallback(device->deviceId, temperatureDelta);
    request.response_value[FSTR_THERMOSTAT_temperature] = temperatureDelta;
    return success;
  }

  if (request.action == FSTR_THERMOSTAT_setThermostatMode && thermostatModeCallback) {
    String thermostatMode = request.request_value[FSTR_THERMOSTAT_thermostatMode] | "";
    success = thermostatModeCallback(device->deviceId, thermostatMode);
    request.response_value[FSTR_THERMOSTAT_thermostatMode] = thermostatMode;
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using ThermostatController = SINRICPRO_NAMESPACE::ThermostatController<T>;