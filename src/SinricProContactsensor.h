/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICCONTACTSENSOR_H_
#define _SINRICCONTACTSENSOR_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProContactsensor :  public SinricProDevice {
  public:
	  SinricProContactsensor(const char* deviceId, unsigned long eventWaitTime=100);

    // event
    bool sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION");

  private:
};

SinricProContactsensor::SinricProContactsensor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime) {}

bool SinricProContactsensor::sendContactEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setContactState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"closed":"open";
  return sendEvent(eventMessage);
}

#endif

