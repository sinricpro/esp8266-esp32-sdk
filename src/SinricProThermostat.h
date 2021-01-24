/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTHERMOSTAT_H_
#define _SINRICTHERMOSTAT_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/ThermostatController.h"
#include "./Controller/TemperatureController.h"

/**
 * @class SinricProThermostat
 * @brief Device to control Thermostat
 * 
 * Support
 * * Set / adjust target temperature
 * * Report target temperature
 * * Report actual temperature
 * * Set thermostat mode `AUTO`, `COOL`, `HEAT`
 **/
class SinricProThermostat :  public SinricProDevice,
                             public PowerStateController,
                             public ThermostatController,
                             public TemperatureController {
  public:
	  SinricProThermostat(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
};

SinricProThermostat::SinricProThermostat(const DeviceId &deviceId) : SinricProDevice(deviceId, "THERMOSTAT"),
                                                                     PowerStateController(this),
                                                                     ThermostatController(this),
                                                                     TemperatureController(this) {}

bool SinricProThermostat::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  if (!success) success = ThermostatController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

