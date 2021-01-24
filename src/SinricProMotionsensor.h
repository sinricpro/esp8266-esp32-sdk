/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICMOTIONSENSOR_H_
#define _SINRICMOTIONSENSOR_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/MotionController.h"

/**
 * @class SinricProMotionsensor
 * @brief Device to report motion detection events
 */
class SinricProMotionsensor :  public SinricProDevice,
                               public PowerStateController,
                               public MotionController {
  public:
	  SinricProMotionsensor(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
};

SinricProMotionsensor::SinricProMotionsensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "MOTION_SENSOR"),
                                                                         PowerStateController(this),
                                                                         MotionController(this) {}
                                                                         
bool SinricProMotionsensor::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

