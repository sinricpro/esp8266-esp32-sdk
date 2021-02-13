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
 * @ingroup Devices
 **/
class SinricProFan : public SinricProDevice,
                     public PowerStateController<SinricProFan>,
                     public PowerLevelController<SinricProFan> {
                     friend class PowerStateController<SinricProFan>;
                     friend class PowerLevelController<SinricProFan>;
  public:
	  SinricProFan(const DeviceId &deviceId);
    bool handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProFan::SinricProFan(const DeviceId &deviceId) : SinricProDevice(deviceId, "FAN_NON-US") {}

bool SinricProFan::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  (void) instance;
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handlePowerLevelController(action, request_value, response_value)) return true;

  return false;
}

#endif

