#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(CONTACT, setContactState);   // "setContactState"
FSTR(CONTACT, state);             // "state"
FSTR(CONTACT, closed);            // "closed"
FSTR(CONTACT, open);              // "open"

/**
 * @brief ContactSensor
 * @ingroup Capabilities
 **/
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

/**
 * \brief Send `setContactState` event to SinricPro Server indicating actual power state
 * 
 * @param detected [in] `bool``true` = contact is closed \n [in] `false` = contact is open
 * @param cause [in] `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return `true` event has been sent successfully
 * @return `false` event has not been sent, maybe you sent to much events in a short distance of time
 **/
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

template <typename T>
using ContactSensor = SINRICPRO_NAMESPACE::ContactSensor<T>;