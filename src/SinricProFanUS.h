/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICFANUS_H_
#define _SINRICFANUS_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/RangeController.h"

/**
 * @class SinricProFanUS
 * @brief Device to control a fan with on / off commands and its speed by a range value
 * @ingroup Devices
 */
class SinricProFanUS :  public SinricProDevice,
                        public PowerStateController<SinricProFanUS>,
                        public RangeController<SinricProFanUS> {
                        friend class PowerStateController<SinricProFanUS>;
                        friend class RangeController<SinricProFanUS>;
  public:
	  SinricProFanUS(const DeviceId &deviceId);
  protected:
    bool handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProFanUS::SinricProFanUS(const DeviceId &deviceId) : SinricProDevice(deviceId, "FAN") {}

bool SinricProFanUS::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handleRangeController(action, instance, request_value, response_value)) return true;

  return false;
}

#endif

