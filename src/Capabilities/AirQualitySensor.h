#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(AIRQUALITY, airQuality);  // "airQuality"
FSTR(AIRQUALITY, pm1);         // "pm1"
FSTR(AIRQUALITY, pm2_5);       // "pm2_5"
FSTR(AIRQUALITY, pm10);        // "pm10"

/**
 * @brief AirQuality
 * @ingroup Capabilities
 **/
template <typename T>
class AirQualitySensor {
  public:
    AirQualitySensor();
    bool sendAirQualityEvent(int pm1 = 0, int pm2_5 = 0, int pm10 = 0, String cause = FSTR_SINRICPRO_PERIODIC_POLL);
  private:
    EventLimiter event_limiter;
};

template <typename T>
AirQualitySensor<T>::AirQualitySensor()
: event_limiter (EVENT_LIMIT_SENSOR_VALUE) {}

/**
 * @brief Sending air quality to SinricPro server
 * 
 * @param   pm1           `int` 1.0 μm particle pollutant	in μg/m3
 * @param   pm2_5         `int` 2.5 μm particle pollutant	in μg/m3
 * @param   pm10          `int` 10 μm particle pollutant in μg/m3
 * @param   cause         (optional) `String` reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool AirQualitySensor<T>::sendAirQualityEvent(int pm1, int pm2_5, int pm10, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);
  
  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_AIRQUALITY_airQuality, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];

  event_value[FSTR_AIRQUALITY_pm1]   = pm1;
  event_value[FSTR_AIRQUALITY_pm2_5] = pm2_5;
  event_value[FSTR_AIRQUALITY_pm10]  = pm10;

  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE

template <typename T>
using AirQualitySensor = SINRICPRO_NAMESPACE::AirQualitySensor<T>;