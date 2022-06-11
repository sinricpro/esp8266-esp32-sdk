/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PushNotification.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/ThermostatController.h"
#include "Capabilities/TemperatureSensor.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

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
                             public PushNotification<SinricProThermostat>,
                             public PowerStateController<SinricProThermostat>,
                             public ThermostatController<SinricProThermostat>,
                             public TemperatureSensor<SinricProThermostat> {
                             friend class SettingController<SinricProThermostat>;
                             friend class PushNotification<SinricProThermostat>;
                             friend class PowerStateController<SinricProThermostat>;
                             friend class ThermostatController<SinricProThermostat>;
                             friend class TemperatureSensor<SinricProThermostat>;
  public:
	  SinricProThermostat(const String &deviceId) : SinricProDevice(deviceId, "THERMOSTAT") {}
};

} // SINRICPRO_NAMESPACE

using SinricProThermostat = SINRICPRO_NAMESPACE::SinricProThermostat;