/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICMOTIONSENSOR_H_
#define _SINRICMOTIONSENSOR_H_

#include "SinricProDevice.h"

/**
 * @class SinricProMotionsensor
 * @brief Device to report motion detection events
 */
class SinricProMotionsensor :  public SinricProDevice {
  public:
	  SinricProMotionsensor(const char* deviceId, unsigned long eventWaitTime=100);

    // event
    bool sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION");
  private:
};

SinricProMotionsensor::SinricProMotionsensor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime) {}

/**
 * @brief Sending motion detection state to SinricPro server
 * 
 * @param   state         `true` if motion has been detected \n 'false' if no motion has been detected
 * @param   cause         (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProMotionsensor::sendMotionEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "motion", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"detected":"notDetected";
  return sendEvent(eventMessage);
}

#endif

