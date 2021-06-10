#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(POWERSENSOR, powerUsage);
FSTR(POWERSENSOR, startTime);
FSTR(POWERSENSOR, voltage);
FSTR(POWERSENSOR, current);
FSTR(POWERSENSOR, power);
FSTR(POWERSENSOR, apparentPower);
FSTR(POWERSENSOR, reactivePower);
FSTR(POWERSENSOR, factor);
FSTR(POWERSENSOR, wattHours);

template <typename T>
class PowerSensor {
  public:
    PowerSensor();
    bool sendPowerSensorEvent(float voltage, float current, float power = -1.0f, float apparentPower = -1.0f, float reactivePower = -1.0f, float factor = -1.0f, String cause = FSTR_SINRICPRO_PERIODIC_POLL);

  private:
    EventLimiter event_limiter;
    unsigned long startTime = 0;
    unsigned long lastPower = 0;
    float getWattHours(unsigned long currentTimestamp);
};

template <typename T>
PowerSensor<T>::PowerSensor()
: event_limiter(EVENT_LIMIT_SENSOR_VALUE) {}

template <typename T>
bool PowerSensor<T>::sendPowerSensorEvent(float voltage, float current, float power, float apparentPower, float reactivePower, float factor, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_POWERSENSOR_powerUsage, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  if (power == -1)
    power = voltage * current;
  if (apparentPower != -1)
    factor = power / apparentPower;

  unsigned long currentTimestamp = device->getTimestamp();

  event_value[FSTR_POWERSENSOR_startTime] = startTime;
  event_value[FSTR_POWERSENSOR_voltage] = voltage;
  event_value[FSTR_POWERSENSOR_current] = current;
  event_value[FSTR_POWERSENSOR_power] = power;
  event_value[FSTR_POWERSENSOR_apparentPower] = apparentPower;
  event_value[FSTR_POWERSENSOR_reactivePower] = reactivePower;
  event_value[FSTR_POWERSENSOR_factor] = factor;
  event_value[FSTR_POWERSENSOR_wattHours] = getWattHours(currentTimestamp);

  startTime = currentTimestamp;
  lastPower = power;
  return device->sendEvent(eventMessage);
}

template <typename T>
float PowerSensor<T>::getWattHours(unsigned long currentTimestamp) {
  if (startTime)
    return (currentTimestamp - startTime) * lastPower / 3600.0f;
  return 0;
}

} // SINRICPRO_NAMESPACE