/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICCONTACTSENSOR_H_
#define _SINRICCONTACTSENSOR_H_

#include "SinricProDevice.h"

/**
 * @class SinricProContactsensor
 * @brief Device to report contact sensor events
 **/
class SinricProContactsensor :  public SinricProDevice {
  public:
	  SinricProContactsensor(const char* deviceId, unsigned long eventWaitTime=100);

    // event
    bool sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION");

  private:
};

SinricProContactsensor::SinricProContactsensor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime) {}

/**
 * \brief Send `setContactState` event to SinricPro Server indicating actual power state
 * 
 * @param state [in] `true` = contact is closed \n [in] `false` = contact is open
 * @param cause [in] `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return `true` event has been sent successfully
 * @return `false` event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProContactsensor::sendContactEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setContactState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"closed":"open";
  return sendEvent(eventMessage);
}

#endif

