/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICBLINDS_H_
#define _SINRICBLINDS_H_

#include "SinricProDevice.h"
#include "Controller/RangeController.h"

/**
 * @class SinricProBlinds
 * @brief Device to control interior blinds
 * 
 * Supporting 
 * * On / Off
 * * Position (0..100)
 * * open / close 
 **/
class SinricProBlinds : public SinricProDevice,
                        public PowerStateController,
                        public RangeController {
  public:
	  SinricProBlinds(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
};

SinricProBlinds::SinricProBlinds(const DeviceId &deviceId) : SinricProDevice(deviceId, "BLIND"),
                                                             PowerStateController(this),
                                                             RangeController(this) {}

bool SinricProBlinds::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  if (!success) success = RangeController::handleRequest(action, request_value, response_value);

  return success;
}


#endif

