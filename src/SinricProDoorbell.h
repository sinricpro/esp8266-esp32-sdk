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
#include "Controller/DoorbellController.h"

/**
 * @class SinricProDoorbell
 * @brief Device to report doorbell events
 *  */
class SinricProDoorbell :  public SinricProDevice,
                           public PowerStateController,
                           public DoorbellController {
  public:
	  SinricProDoorbell(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
};

SinricProDoorbell::SinricProDoorbell(const DeviceId &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR"),
                                                                 PowerStateController(this),
                                                                 DoorbellController(this) {}

bool SinricProDoorbell::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

