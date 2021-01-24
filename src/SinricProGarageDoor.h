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
 * 
 * Supporting 
 * * open / close 
 **/
class SinricProGarageDoor : public SinricProDevice,
                            public ModeController {
  public:
	  SinricProGarageDoor(const DeviceId &deviceId);
        
    bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProGarageDoor::SinricProGarageDoor(const DeviceId &deviceId) : SinricProDevice(deviceId, "GARAGE_DOOR"),
                                                                     ModeController(this) {}

bool SinricProGarageDoor::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = ModeController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

