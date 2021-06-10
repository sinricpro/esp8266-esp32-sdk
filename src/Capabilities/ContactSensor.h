#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(CONTACT, setContactState);
FSTR(CONTACT, state);
FSTR(CONTACT, closed);
FSTR(CONTACT, open);

template <typename T>
class ContactSensor {
  public:
    ContactSensor();
    bool sendContactEvent(bool detected, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
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

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_CONTACT_setContactState, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_CONTACT_state] = detected ? FSTR_CONTACT_closed : FSTR_CONTACT_open;
  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE