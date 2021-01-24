#ifndef _CONTACTCONTROLLER_H_
#define _CONTACTCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class ContactController {
  public:
    ContactController(SinricProDeviceInterface *device);
    bool sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION");
  protected:
  private:
    SinricProDeviceInterface *device;
};

ContactController::ContactController(SinricProDeviceInterface *device) : device(device) {}

/**
 * \brief Send `setContactState` event to SinricPro Server indicating actual power state
 * 
 * @param state [in] `true` = contact is closed \n [in] `false` = contact is open
 * @param cause [in] `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return `true` event has been sent successfully
 * @return `false` event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool ContactController::sendContactEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("setContactState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "closed" : "open";
  return device->sendEvent(eventMessage);
}

#endif