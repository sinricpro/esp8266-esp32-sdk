#pragma once

#include <WString.h>
#include "../EventLimiter.h"
#include "../SinricProConfig.h"
#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

template <typename T>
class AirQualitySensor {
  public:
    AirQualitySensor();
    bool sendAirQualityEvent(int pm1 = 0, int pm2_5 = 0, int pm10 = 0, String cause = "PERIODIC_POLL");
  private:
    EventLimiter event_limiter;
};

template <typename T>
AirQualitySensor<T>::AirQualitySensor()
: event_limiter(EVENT_LIMIT_SENSOR_VALUE) {}

template <typename T>
bool AirQualitySensor<T>::sendAirQualityEvent(int pm1, int pm2_5, int pm10, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);
  
  DynamicJsonDocument eventMessage = device->prepareEvent("airQuality", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];

  event_value["pm1"] = pm1;
  event_value["pm2_5"] = pm2_5;
  event_value["pm10"] = pm10;

  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE