#ifndef _CONTACTEVENTSOURCE_H_
#define _CONTACTEVENTSOURCE_H_

/**
 * @brief ContactEventSource
 * @ingroup EventSource
 **/
template <typename T>
class ContactEventSource {
  public:
    bool sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION");
};

/**
 * \brief Send `setContactState` event to SinricPro Server indicating actual power state
 * 
 * @param detected [in] `bool``true` = contact is closed \n [in] `false` = contact is open
 * @param cause [in] `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return `true` event has been sent successfully
 * @return `false` event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ContactEventSource<T>::sendContactEvent(bool detected, String cause) {
  T& device = static_cast<T&>(*this);
  
  DynamicJsonDocument eventMessage = device.prepareEvent("setContactState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = detected ? "closed" : "open";
  return device.sendEvent(eventMessage);
}

#endif