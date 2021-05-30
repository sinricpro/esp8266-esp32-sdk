/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICPOWERSENSOR_H_
#define _SINRICPOWERSENSOR_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerSensor.h"
#include "Capabilities/PowerStateController.h"

/**
 * @class SinricProPowerSensor
 * @brief Device to report power usage
 * @ingroup Devices
 **/
class SinricProPowerSensor :  public SinricProDevice,
                              public SettingController<SinricProPowerSensor>,
                              public PowerSensor<SinricProPowerSensor>,
                              public PowerStateController<SinricProPowerSensor> {
                              friend class SettingController<SinricProPowerSensor>;
                              friend class PowerSensor<SinricProPowerSensor>;
                              friend class PowerStateController<SinricProPowerSensor>;
  public:
	  SinricProPowerSensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "POWER_SENSOR") {}
};

#endif