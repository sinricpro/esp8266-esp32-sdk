/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICSWITCH_H_
#define _SINRICSWITCH_H_

#include "./SinricProDevice.h"
#include "./Controller/PowerStateController.h"

/**
 * @class SinricProSwitch
 * @brief Device suporting basic on / off command
 * @ingroup Devices
 **/
class SinricProSwitch :  public SinricProDevice,
                         public PowerStateController<SinricProSwitch> {
  public:
    SinricProSwitch(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProSwitch::SinricProSwitch(const DeviceId &deviceId) : SinricProDevice(deviceId, "SWITCH") {
}

bool SinricProSwitch::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  return false;
}

#endif

