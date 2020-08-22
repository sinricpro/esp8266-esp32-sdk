/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDOORBELL_H_
#define _SINRICDOORBELL_H_

#include "SinricProDevice.h"

/**
 * @class SinricProDoorbell
 * @brief Device to report doorbell events
 *  */
class SinricProDoorbell :  public SinricProDevice {
  public:
	  SinricProDoorbell(const DeviceId &deviceId);
    String getProductType() { return SinricProDevice::getProductType() + String("DOORBELL"); }
    // event
    bool sendDoorbellEvent(String cause = "PHYSICAL_INTERACTION");
  private:
};

SinricProDoorbell::SinricProDoorbell(const DeviceId &deviceId) : SinricProDevice(deviceId) {}

/**
 * @brief Send Doorbell event to SinricPro Server indicating someone pressed the doorbell button
 * 
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProDoorbell::sendDoorbellEvent(String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "DoorbellPress", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = "pressed";
  return sendEvent(eventMessage);
}

#endif

