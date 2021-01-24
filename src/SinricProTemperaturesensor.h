/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTEMPERATURESENSOR_H_
#define _SINRICTEMPERATURESENSOR_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/TemperatureController.h"

/**
 * @class SinricProTemperaturesensor
 * @brief Device to report actual temperature and humidity
 */
class SinricProTemperaturesensor :  public SinricProDevice,
                                    public PowerStateController,
                                    public TemperatureController {
  public:
	  SinricProTemperaturesensor(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProTemperaturesensor::SinricProTemperaturesensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "TEMPERATURESENSOR"),
                                                                                   PowerStateController(this),
                                                                                   TemperatureController(this) {}

bool SinricProTemperaturesensor::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

