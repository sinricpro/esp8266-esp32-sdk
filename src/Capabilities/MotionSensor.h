#pragma once

#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(MOTION, motion);
FSTR(MOTION, state);
FSTR(MOTION, detected);
FSTR(MOTION, notDetected);

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