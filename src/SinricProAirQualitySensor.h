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
#include "Capabilities/AirQualitySensor.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProAirQualitySensor
 * @brief Device to report air quality events
 * @ingroup Devices
 */
class SinricProAirQualitySensor : public SinricProDevice,
                                  public SettingController<SinricProAirQualitySensor>,
                                  public PushNotification<SinricProAirQualitySensor>, 
                                  public PowerStateController<SinricProAirQualitySensor>,
                                  public AirQualitySensor<SinricProAirQualitySensor> {
                                  friend class SettingController<SinricProAirQualitySensor>;
                                  friend class PushNotification<AirQualitySensor>;
                                  friend class PowerStateController<SinricProAirQualitySensor>;
                                  friend class AirQualitySensor<SinricProAirQualitySensor>;
public:
  SinricProAirQualitySensor(const String &deviceId) : SinricProDevice(deviceId, "AIR_QUALITY_SENSOR"){};
};

} // SINRICPRO_NAMESPACE

using SinricProAirQualitySensor = SINRICPRO_NAMESPACE::SinricProAirQualitySensor;