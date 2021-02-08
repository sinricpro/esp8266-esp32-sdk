#ifndef _MOTIONEVENTSOURCE_H_
#define _MOTIONEVENTSOURCE_H_

/**
 * @brief MotionEventSource
 * @ingroup EventSource
 **/
template <typename T>
class MotionEventSource {
  public:
    bool sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION");
};

/**
 * @brief Sending motion detection state to SinricPro server
 * 
 * @param   state         `true` if motion has been detected \n 'false' if no motion has been detected
 * @param   cause         (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool MotionEventSource<T>::sendMotionEvent(bool state, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("motion", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "detected" : "notDetected";
  return device.sendEvent(eventMessage);
}

#endif