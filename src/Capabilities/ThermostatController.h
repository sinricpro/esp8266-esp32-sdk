#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(THERMOSTAT, setThermostatMode);
FSTR(THERMOSTAT, thermostatMode);
FSTR(THERMOSTAT, targetTemperature);
FSTR(THERMOSTAT, temperature);
FSTR(THERMOSTAT, adjustTargetTemperature);

using ThermostatModeCallback = std::function<bool(const String &, String &)>;
using SetTargetTemperatureCallback = std::function<bool(const String &, float &)>;
using AdjustTargetTemperatureCallback = std::function<bool(const String &, float &)>;

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
    virtual bool onThermostatMode(String &mode);
    virtual bool onTargetTemperature(float &targetTemperature);
    virtual bool onAdjustTargetTemperature(float &targetTemperatureDelta);
    bool handleThermostatController(SinricProRequest &request);

  private:
    EventLimiter eventLimiter_targetTemperature;
    EventLimiter eventLimiter_thermostatMode;
    ThermostatModeCallback thermostatModeCallback;
    SetTargetTemperatureCallback targetTemperatureCallback;
    AdjustTargetTemperatureCallback adjustTargetTemperatureCallback;
};

template <typename T>
ThermostatController<T>::ThermostatController()
: eventLimiter_targetTemperature(EVENT_LIMIT_STATE)
, eventLimiter_thermostatMode(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&ThermostatController<T>::handleThermostatController, this, std::placeholders::_1)); 
}

template <typename T>
void ThermostatController<T>::onThermostatMode(ThermostatModeCallback cb) {
  thermostatModeCallback = cb;
}

template <typename T>
void ThermostatController<T>::onTargetTemperature(SetTargetTemperatureCallback cb) {
  targetTemperatureCallback = cb;
}

template <typename T>
void ThermostatController<T>::onAdjustTargetTemperature(AdjustTargetTemperatureCallback cb) {
  adjustTargetTemperatureCallback = cb;
}

template <typename T>
bool ThermostatController<T>::onThermostatMode(String &mode) {
  T* device = static_cast<T*>(this);
  if (thermostatModeCallback) return thermostatModeCallback(device->deviceId, mode);
  return false;
}

template <typename T>
bool ThermostatController<T>::onTargetTemperature(float &targetTemperature) {
  T* device = static_cast<T*>(this);
  if (targetTemperatureCallback) return targetTemperatureCallback(device->deviceId, targetTemperature);
  return false;
}

template <typename T>
bool ThermostatController<T>::onAdjustTargetTemperature(float &targetTemperatureDelta) {
  T* device = static_cast<T*>(this);
  if (adjustTargetTemperatureCallback) return adjustTargetTemperatureCallback(device->deviceId, targetTemperatureDelta);
  return false;
}

template <typename T>
bool ThermostatController<T>::sendThermostatModeEvent(String thermostatMode, String cause) {
  if (eventLimiter_thermostatMode) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_THERMOSTAT_setThermostatMode, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_THERMOSTAT_thermostatMode] = thermostatMode;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ThermostatController<T>::sendTargetTemperatureEvent(float temperature, String cause) {
  if (eventLimiter_targetTemperature) return false;
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent(FSTR_THERMOSTAT_targetTemperature, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_THERMOSTAT_temperature] = roundf(temperature * 10) / 10.0;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool ThermostatController<T>::handleThermostatController(SinricProRequest &request) {
  bool success = false;

  if (request.action == FSTR_THERMOSTAT_targetTemperature) {
    float temperature;
    if (request.request_value.containsKey(FSTR_THERMOSTAT_temperature))  {
      temperature = request.request_value[FSTR_THERMOSTAT_temperature];
    }  else {
      temperature = 1;
    }
    success = onTargetTemperature(temperature);
    request.response_value[FSTR_THERMOSTAT_temperature] = temperature;
    return success;
  }

  if (request.action == FSTR_THERMOSTAT_adjustTargetTemperature) {
    float temperatureDelta = request.request_value[FSTR_THERMOSTAT_temperature];
    success = onAdjustTargetTemperature(temperatureDelta);
    request.response_value[FSTR_THERMOSTAT_temperature] = temperatureDelta;
    return success;
  }

  if (request.action == FSTR_THERMOSTAT_setThermostatMode) {
    String thermostatMode = request.request_value[FSTR_THERMOSTAT_thermostatMode] | "";
    success = onThermostatMode(thermostatMode);
    request.response_value[FSTR_THERMOSTAT_thermostatMode] = thermostatMode;
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACe