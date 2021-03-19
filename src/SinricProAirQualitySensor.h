/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICAIRQUALITYSENSOR_H_
#define _SINRICAIRQUALITYSENSOR_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/AirQualitySensor.h"

/**
 * @class SinricProAirQualitySensor
 * @brief Device to report air quality events
 * @ingroup Devices
 */
class SinricProAirQualitySensor : public SinricProDevice,
                                  public SettingController<SinricProAirQualitySensor>,
                                  public PowerStateController<SinricProAirQualitySensor>,
                                  public AirQualitySensor<SinricProAirQualitySensor> {
                                  friend class SettingController<SinricProAirQualitySensor>;
                                  friend class PowerStateController<SinricProAirQualitySensor>;
                                  friend class AirQualitySensor<SinricProAirQualitySensor>;
                                  
public:
  SinricProAirQualitySensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "AIR_QUALITY_SENSOR"){};
};

#endif

