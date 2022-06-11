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
#include "Capabilities/TemperatureSensor.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProTemperaturesensor
 * @brief Device to report actual temperature and humidity
 * @ingroup Devices
 */
class SinricProTemperaturesensor :  public SinricProDevice,
                                    public SettingController<SinricProTemperaturesensor>,
                                    public PushNotification<SinricProTemperaturesensor>,
                                    public PowerStateController<SinricProTemperaturesensor>,
                                    public TemperatureSensor<SinricProTemperaturesensor> {
                                    friend class SettingController<SinricProTemperaturesensor>;
                                    friend class PushNotification<SinricProTemperaturesensor>;
                                    friend class PowerStateController<SinricProTemperaturesensor>;
                                    friend class TemperatureSensor<SinricProTemperaturesensor>;
  public:
	  SinricProTemperaturesensor(const String &deviceId) : SinricProDevice(deviceId, "TEMPERATURESENSOR") {}
};

} // SINRICPRO_NAMESPACE

using SinricProTemperaturesensor = SINRICPRO_NAMESPACE::SinricProTemperaturesensor;