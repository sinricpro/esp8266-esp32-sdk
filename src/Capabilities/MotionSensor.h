#ifndef _MOTIONSENSOR_H_
#define _MOTIONSENSOR_H_

/**
 * @brief MotionSensor
 * @ingroup Capabilities
 **/
template <typename T>
class MotionSensor {
  public:
    bool sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION");
};

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
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("motion", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = detected ? "detected" : "notDetected";
  return device.sendEvent(eventMessage);
}

#endif