#pragma once

#include "../EventLimiter.h"
#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @brief MotionSensor
 * @ingroup Capabilities
 **/
template <typename T>
class MotionSensor {
  public:
    MotionSensor();
    bool sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION");
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

  DynamicJsonDocument eventMessage = device->prepareEvent("motion", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = detected ? "detected" : "notDetected";
  return device->sendEvent(eventMessage);
}

} // SINRICPRO_NAMESPACE