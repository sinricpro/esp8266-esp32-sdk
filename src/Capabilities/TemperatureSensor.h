#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(TEMPERATURE, currentTemperature);    // "currentTemperature"
FSTR(TEMPERATURE, humidity);              // "humidity"
FSTR(TEMPERATURE, temperature);           // "temperature"

/**
 * @brief TemperatureSensor
 * @ingroup Capabilities
 **/
template <typename T>
class TemperatureSensor {
  public:
    TemperatureSensor();
    bool sendTemperatureEvent(float temperature, float humidity = -1, String cause = FSTR_SINRICPRO_PERIODIC_POLL);
  private:
    EventLimiter event_limiter;
};

template <typename T>
TemperatureSensor<T>::TemperatureSensor() 
: event_limiter(EVENT_LIMIT_SENSOR_VALUE) {}

/**
 * @brief Send `currentTemperature` event to report actual temperature (measured by a sensor)
 * 
 * @param   temperature   `float` actual temperature measured by a sensor
 * @param   humidity      `float` (optional) actual humidity measured by a sensor (default=-1.0f means not supported)
 * @param   cause         (optional) `String` reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the even
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool TemperatureSensor<T>::sendTemperatureEvent(float temperature, float humidity, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_TEMPERATURE_currentTemperature, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_TEMPERATURE_humidity] = roundf(humidity * 100) / 100.0;
  event_value[FSTR_TEMPERATURE_temperature] = roundf(temperature * 10) / 10.0;
  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE

template <typename T>
using TemperatureSensor = SINRICPRO_NAMESPACE::TemperatureSensor<T>;