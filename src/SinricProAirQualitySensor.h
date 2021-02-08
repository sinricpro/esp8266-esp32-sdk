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
#include "./EventSource/AirQualityEventSource.h"

/**
 * @class SinricProAirQualitySensor
 * @brief Device to report air quality events
 * @ingroup Devices
 */
class SinricProAirQualitySensor : public SinricProDevice,
                                  public PowerStateController<SinricProAirQualitySensor>,
                                  public AirQualityEventSource<SinricProAirQualitySensor> {
public:
  SinricProAirQualitySensor(const DeviceId &deviceId);
  bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProAirQualitySensor::SinricProAirQualitySensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "AIR_QUALITY_SENSOR") {}

bool SinricProAirQualitySensor::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  return false;
}

#endif

