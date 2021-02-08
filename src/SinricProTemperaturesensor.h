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
#include "./EventSource/TemperatureEventSource.h"

/**
 * @class SinricProTemperaturesensor
 * @brief Device to report actual temperature and humidity
 * @ingroup Devices
 */
class SinricProTemperaturesensor :  public SinricProDevice,
                                    public PowerStateController<SinricProTemperaturesensor>,
                                    public TemperatureEventSource<SinricProTemperaturesensor> {
  public:
	  SinricProTemperaturesensor(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProTemperaturesensor::SinricProTemperaturesensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "TEMPERATURESENSOR") {}

bool SinricProTemperaturesensor::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  return false;
}

#endif

