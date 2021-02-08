/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICGARAGEDOOR_H_
#define _SINRICGARAGEDOOR_H_

#include "SinricProDevice.h"
#include "Controller/ModeController.h"

/**
 * @class SinricProGarageDoor
 * @brief Device to control a garage door
 * @ingroup Devices
 * 
 * Supporting 
 * * open / close 
 **/
class SinricProGarageDoor : public SinricProDevice,
                            public ModeController<SinricProGarageDoor> {
  public:
	  SinricProGarageDoor(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProGarageDoor::SinricProGarageDoor(const DeviceId &deviceId) : SinricProDevice(deviceId, "GARAGE_DOOR") {}

bool SinricProGarageDoor::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handleModeController(action, instance, request_value, response_value)) return true;
  return false;
}

#endif

