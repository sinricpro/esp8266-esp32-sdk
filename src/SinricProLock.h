/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLOCK_H_
#define _SINRICLOCK_H_

#include "SinricProDevice.h"
#include "./Controller/LockController.h"

/**
 * @class SinricProLock
 * @brief Device to control a smart lock
 * @ingroup Devices
 * 
 * Supporting 
 * * on / off
 * * lock / unlock
 **/
class SinricProLock :  public SinricProDevice,
                       public LockController<SinricProLock> {
  public:
	  SinricProLock(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProLock::SinricProLock(const DeviceId &deviceId) : SinricProDevice(deviceId, "SMARTLOCK") {}

bool SinricProLock::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handleLockController(action, request_value, response_value)) return true;

  return false;
}

#endif

