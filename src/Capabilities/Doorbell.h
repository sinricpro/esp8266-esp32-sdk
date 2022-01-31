#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(DOORBELL, DoorbellPress);    // "DoorbellPress"
FSTR(DOORBELL, state);            // "state"
FSTR(DOORBELL, pressed);          // "pressed"

/**
 * @brief Dorbell
 * @ingroup Capabilities
 **/
template <typename T>
class Doorbell {
  public:
    Doorbell();
    bool sendDoorbellEvent(String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
  private:
    EventLimiter event_limiter;
};

template <typename T>
Doorbell<T>::Doorbell()
: event_limiter(EVENT_LIMIT_SENSOR_STATE) {}

/**
 * @brief Send Doorbell event to SinricPro Server indicating someone pressed the doorbell button
 * 
 * @param   cause         `String` (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool Doorbell<T>::sendDoorbellEvent(String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_DOORBELL_DoorbellPress, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_DOORBELL_state] = FSTR_DOORBELL_pressed;
  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE

template <typename T>
using Doorbell = SINRICPRO_NAMESPACE::Doorbell<T>;