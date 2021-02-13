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
#include "./EventSource/TemperatureEventSource.h"

/**
 * @class SinricProThermostat
 * @brief Device to control Thermostat
 * @ingroup Devices
 * 
 * Support
 * * Set / adjust target temperature
 * * Report target temperature
 * * Report actual temperature
 * * Set thermostat mode `AUTO`, `COOL`, `HEAT`
 **/
class SinricProThermostat :  public SinricProDevice,
                             public PowerStateController<SinricProThermostat>,
                             public ThermostatController<SinricProThermostat>,
                             public TemperatureEventSource<SinricProThermostat> {
                             friend class PowerStateController<SinricProThermostat>;
                             friend class ThermostatController<SinricProThermostat>;
                             friend class TemperatureEventSource<SinricProThermostat>;
  public:
	  SinricProThermostat(const DeviceId &deviceId);
  protected:
    bool handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProThermostat::SinricProThermostat(const DeviceId &deviceId) : SinricProDevice(deviceId, "THERMOSTAT") {}

bool SinricProThermostat::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  (void) instance;
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handleThermostatController(action, request_value, response_value)) return true;

  return false;
}

#endif

