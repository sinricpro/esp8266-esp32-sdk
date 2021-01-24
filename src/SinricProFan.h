/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICFAN_H_
#define _SINRICFAN_H_

#include "SinricProDevice.h"
#include "SinricProDimSwitch.h"

#include "./Controller/PowerStateController.h"
#include "./Controller/PowerLevelController.h"

/**
 * @class SinricProFan
 * @brief Device to turn on / off a fan and change it's speed by using powerlevel
 **/
class SinricProFan : public SinricProDevice,
                     public PowerStateController,
                     public PowerLevelController {
  public:
	  SinricProFan(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);
};

SinricProFan::SinricProFan(const DeviceId &deviceId) : SinricProDevice(deviceId, "FAN_NON-US"),
                                                       PowerStateController(this),
                                                       PowerLevelController(this) {}

bool SinricProFan::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  if (!success) success = PowerLevelController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

