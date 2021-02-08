
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
 * @ingroup Devices
 **/
class SinricProCamera : public SinricProDevice,
                        public PowerStateController<SinricProCamera> {
  public:
	  SinricProCamera(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProCamera::SinricProCamera(const DeviceId &deviceId) : SinricProDevice(deviceId, "CAMERA") {}

bool SinricProCamera::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  return false;
}

#endif

