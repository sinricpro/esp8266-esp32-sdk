#pragma once

#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

template <typename T>
class ContactSensor {
  public:
    ContactSensor();
    bool sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION");
  private:
    EventLimiter event_limiter;
};

template <typename T>
ContactSensor<T>::ContactSensor()
: event_limiter(EVENT_LIMIT_SENSOR_STATE) {}

template <typename T>
bool ContactSensor<T>::sendContactEvent(bool detected, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setContactState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = detected ? "closed" : "open";
  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE