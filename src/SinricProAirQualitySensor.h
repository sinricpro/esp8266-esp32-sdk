/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICAIRQUALITYSENSOR_H_
#define _SINRICAIRQUALITYSENSOR_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/AirQualityController.h"

/**
 * @class SinricProAirQualitySensor
 * @brief Device to report air quality events
 */
class SinricProAirQualitySensor :  public SinricProDevice,
                                   public PowerStateController,
                                   public AirQualityController {
  public:
	  SinricProAirQualitySensor(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);
};

SinricProAirQualitySensor::SinricProAirQualitySensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "AIR_QUALITY_SENSOR"),
                                                                                 PowerStateController(this),
                                                                                 AirQualityController(this) {}

bool SinricProAirQualitySensor::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  return success;
}

#endif

