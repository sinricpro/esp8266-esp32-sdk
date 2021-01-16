/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDIMSWITCH_H_
#define _SINRICDIMSWITCH_H_

#include "SinricProDevice.h"
#include "./Controller/PowerstateController.h"
#include "./Controller/PowerLevelController.h"

/**
 * @class SinricProDimSwitch
 * @brief Device which supports on / off and dimming commands
 **/
class SinricProDimSwitch :  public SinricProDevice,
                            public PowerStateController,
                            public PowerLevelController {
  public:
	  SinricProDimSwitch(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
    String getProductType() { return SinricProDevice::getProductType() + String("DIMMABLE_SWITCH"); }
};

SinricProDimSwitch::SinricProDimSwitch(const DeviceId &deviceId) : 
  SinricProDevice(deviceId),
  PowerStateController(this),
  PowerLevelController(this) {
}

bool SinricProDimSwitch::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  if (deviceId != this->deviceId) return false;

  bool success = false;
  if (!success) PowerStateController::handleRequest(action, request_value, response_value);
  if (!success) PowerLevelController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

