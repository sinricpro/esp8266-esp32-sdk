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
 **/
class SinricProSwitch :  public SinricProDevice,
                         public PowerStateController {
  public:
    SinricProSwitch(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);
    String getProductType();
};

SinricProSwitch::SinricProSwitch(const DeviceId &deviceId) : 
  SinricProDevice(deviceId), 
  PowerStateController(this) {
}

bool SinricProSwitch::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  if (deviceId != this->deviceId) return false;

  bool success = false;
  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  return success;
}

String SinricProSwitch::getProductType(){ 
  return SinricProDevice::getProductType() + String("SWITCH"); 
}

#endif

