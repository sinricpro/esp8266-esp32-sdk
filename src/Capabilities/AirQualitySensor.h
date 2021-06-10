#pragma once

#include <WString.h>
#include "../EventLimiter.h"
#include "../SinricProConfig.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(AIRQUALITY, airQuality);
FSTR(AIRQUALITY, pm1);
FSTR(AIRQUALITY, pm2_5);
FSTR(AIRQUALITY, pm10);

/**
 * @brief AirQualitySensor
 * @ingroup Capabilities
 * Supports
 * - @ref sendAirQualityEvent
 */
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
: event_limiter(EVENT_LIMIT_SENSOR_VALUE) {}

/**
 * @brief Update the AirQuality on SinricPro Server
 * 
 * @param pm1 pm1 level
 * @param pm2_5  pm2.5 level
 * @param pm10 pm10 level
 * @param cause default: "PERIODIC_POLL"
 * @return true event was sent to SinricProServer
 * @return false event could not be send to SinricProServer
 */
template <typename T>
bool AirQualitySensor<T>::sendAirQualityEvent(int pm1, int pm2_5, int pm10, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);
  
  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_AIRQUALITY_airQuality, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];

  event_value[FSTR_AIRQUALITY_pm1] = pm1;
  event_value[FSTR_AIRQUALITY_pm2_5] = pm2_5;
  event_value[FSTR_AIRQUALITY_pm10] = pm10;

  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE