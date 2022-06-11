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
#include "Capabilities/PowerSensor.h"
#include "Capabilities/PowerStateController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProPowerSensor
 * @brief Device to report power usage
 * @ingroup Devices
 **/
class SinricProPowerSensor :  public SinricProDevice,
                              public SettingController<SinricProPowerSensor>,
                              public PushNotification<SinricProPowerSensor>,
                              public PowerSensor<SinricProPowerSensor>,
                              public PowerStateController<SinricProPowerSensor> {
                              friend class SettingController<SinricProPowerSensor>;
                              friend class PushNotification<SinricProPowerSensor>;
                              friend class PowerSensor<SinricProPowerSensor>;
                              friend class PowerStateController<SinricProPowerSensor>;
  public:
	  SinricProPowerSensor(const String &deviceId) : SinricProDevice(deviceId, "POWER_SENSOR") {}
};

} // SINRICPRO_NAMESPACE

using SinricProPowerSensor = SINRICPRO_NAMESPACE::SinricProPowerSensor;