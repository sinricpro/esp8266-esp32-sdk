/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICPOWERSENSOR_H_
#define _SINRICPOWERSENSOR_H_

#include "SinricProDevice.h"
#include "Controller/PowerSensorController.h"

/**
 * @class SinricProPowerSensor
 * @brief Device to report power usage
 *  */
class SinricProPowerSensor :  public SinricProDevice,
                              public PowerSensorController {
  public:
	  SinricProPowerSensor(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);
};

SinricProPowerSensor::SinricProPowerSensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "POWER_SENSOR"),
                                                                       PowerSensorController(this) {}

bool SinricProPowerSensor::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) { return false; }

#endif