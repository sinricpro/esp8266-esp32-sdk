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
#include "Capabilities/MotionSensor.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProMotionsensor
 * @brief Device to report motion detection events
 * @ingroup Devices
 */
class SinricProMotionsensor : public SinricProDevice,
                              public SettingController<SinricProMotionsensor>,
                              public PushNotification<SinricProMotionsensor>,
                              public PowerStateController<SinricProMotionsensor>,
                              public MotionSensor<SinricProMotionsensor> {
                              friend class SettingController<SinricProMotionsensor>;
                              friend class PushNotification<SinricProMotionsensor>;
                              friend class PowerStateController<SinricProMotionsensor>;
                              friend class MotionSensor<SinricProMotionsensor>;
  public:
    SinricProMotionsensor(const String &deviceId) : SinricProDevice(deviceId, "MOTION_SENSOR") {}
};

} // SINRICPRO_NAMESPACE

using SinricProMotionsensor = SINRICPRO_NAMESPACE::SinricProMotionsensor;