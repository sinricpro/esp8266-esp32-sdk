#ifndef _CONTACTSENSOR_H_
#define _CONTACTSENSOR_H_

/**
 * @brief ContactSensor
 * @ingroup Capabilities
 **/
template <typename T>
class ContactSensor {
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
bool ContactSensor<T>::sendContactEvent(bool detected, String cause) {
  T& device = static_cast<T&>(*this);
  
  DynamicJsonDocument eventMessage = device.prepareEvent("setContactState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = detected ? "closed" : "open";
  return device.sendEvent(eventMessage);
}

#endif