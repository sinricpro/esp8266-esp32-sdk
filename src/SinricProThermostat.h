/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTHERMOSTAT_H_
#define _SINRICTHERMOSTAT_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/ThermostatController.h"
#include "Capabilities/TemperatureSensor.h"

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
                             public SettingController<SinricProThermostat>,
                             public PowerStateController<SinricProThermostat>,
                             public ThermostatController<SinricProThermostat>,
                             public TemperatureSensor<SinricProThermostat> {
                             friend class SettingController<SinricProThermostat>;
                             friend class PowerStateController<SinricProThermostat>;
                             friend class ThermostatController<SinricProThermostat>;
                             friend class TemperatureSensor<SinricProThermostat>;
  public:
	  SinricProThermostat(const DeviceId &deviceId) : SinricProDevice(deviceId, "THERMOSTAT") {}
};

#endif

