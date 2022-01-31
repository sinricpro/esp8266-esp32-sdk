#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(MOTION, motion);         // "motion"
FSTR(MOTION, state);          // "state"
FSTR(MOTION, detected);       // "detected"
FSTR(MOTION, notDetected);    // "notDetected"

/**
 * @brief MotionSensor
 * @ingroup Capabilities
 **/
template <typename T>
class MotionSensor {
  public:
    MotionSensor();
    bool sendMotionEvent(bool detected, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
  private:
    EventLimiter event_limiter;
};

template <typename T>
MotionSensor<T>::MotionSensor()
: event_limiter(EVENT_LIMIT_SENSOR_STATE) {}

/**
 * @brief Sending motion detection state to SinricPro server
 * 
 * @param   detected      `bool` `true` if motion has been detected \n 'false' if no motion has been detected
 * @param   cause         (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool MotionSensor<T>::sendMotionEvent(bool detected, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_MOTION_motion, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_MOTION_state] = detected ? FSTR_MOTION_detected : FSTR_MOTION_notDetected;
  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE

template <typename T>
using MotionSensor = SINRICPRO_NAMESPACE::MotionSensor<T>;