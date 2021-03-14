/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTEMPERATURESENSOR_H_
#define _SINRICTEMPERATURESENSOR_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/TemperatureSensor.h"

/**
 * @class SinricProTemperaturesensor
 * @brief Device to report actual temperature and humidity
 * @ingroup Devices
 */
class SinricProTemperaturesensor :  public SinricProDevice,
                                    public SettingController<SinricProTemperaturesensor>,
                                    public PowerStateController<SinricProTemperaturesensor>,
                                    public TemperatureSensor<SinricProTemperaturesensor> {
                                    friend class SettingController<SinricProTemperaturesensor>;
                                    friend class PowerStateController<SinricProTemperaturesensor>;
                                    friend class TemperatureSensor<SinricProTemperaturesensor>;
  public:
	  SinricProTemperaturesensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "TEMPERATURESENSOR") {}
};

#endif

