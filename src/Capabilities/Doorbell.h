#pragma once

#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

template <typename T>
class Doorbell {
  public:
    Doorbell();
    bool sendDoorbellEvent(String cause = "PHYSICAL_INTERACTION");
  private:
    EventLimiter event_limiter;
};

template <typename T>
Doorbell<T>::Doorbell()
: event_limiter(EVENT_LIMIT_SENSOR_STATE) {}

template <typename T>
bool Doorbell<T>::sendDoorbellEvent(String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("DoorbellPress", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = "pressed";
  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE