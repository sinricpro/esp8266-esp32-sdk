/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDOORBELL_H_
#define _SINRICDOORBELL_H_

#include "SinricProDevice.h"
#include "Controller/PowerStateController.h"
#include "EventSource/DoorbellEventSource.h"

/**
 * @class SinricProDoorbell
 * @brief Device to report doorbell events
 * @ingroup Devices
 **/
class SinricProDoorbell :  public SinricProDevice,
                           public PowerStateController<SinricProDoorbell>,
                           public DoorbellEventSource<SinricProDoorbell> {
  public:
	  SinricProDoorbell(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);

  private:
};

SinricProDoorbell::SinricProDoorbell(const DeviceId &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR") {}

bool SinricProDoorbell::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  return false;
}

#endif

