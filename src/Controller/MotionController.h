#ifndef _MOTIONCONTROLLER_H_
#define _MOTIONCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class MotionController {
  public:
    MotionController(SinricProDeviceInterface* device);

    bool sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION");

  private:
    SinricProDeviceInterface *device;
};

MotionController::MotionController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Sending motion detection state to SinricPro server
 * 
 * @param   state         `true` if motion has been detected \n 'false' if no motion has been detected
 * @param   cause         (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool MotionController::sendMotionEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("motion", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "detected" : "notDetected";
  return device->sendEvent(eventMessage);
}

#endif