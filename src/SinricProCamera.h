
/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICCAMERA_H_
#define _SINRICCAMERA_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"

/**
 * @class SinricProCamera
 * @brief Camera suporting basic on / off command
 **/
class SinricProCamera : public SinricProDevice,
                        public PowerStateController {
  public:
	  SinricProCamera(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);
};

SinricProCamera::SinricProCamera(const DeviceId &deviceId) : SinricProDevice(deviceId, "CAMERA"),
                                                             PowerStateController(this) {}

bool SinricProCamera::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

