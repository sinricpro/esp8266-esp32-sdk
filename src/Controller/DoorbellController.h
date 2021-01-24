#ifndef _DOORBELLCONTROLLER_H_
#define _DOORBELLCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class DoorbellController {
  public:
    DoorbellController(SinricProDeviceInterface *device);
    bool sendDoorbellEvent(String cause = "PHYSICAL_INTERACTION");
  protected : 
  private : 
    SinricProDeviceInterface *device;
};

DoorbellController::DoorbellController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Send Doorbell event to SinricPro Server indicating someone pressed the doorbell button
 * 
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool DoorbellController::sendDoorbellEvent(String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("DoorbellPress", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = "pressed";
  return device->sendEvent(eventMessage);
}

#endif