/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICMOTIONSENSOR_H_
#define _SINRICMOTIONSENSOR_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/MotionSensor.h"

/**
 * @class SinricProMotionsensor
 * @brief Device to report motion detection events
 * @ingroup Devices
 */
class SinricProMotionsensor : public SinricProDevice,
                              public SettingController<SinricProMotionsensor>,
                              public PowerStateController<SinricProMotionsensor>,
                              public MotionSensor<SinricProMotionsensor> {
                              friend class SettingController<SinricProMotionsensor>;
                              friend class PowerStateController<SinricProMotionsensor>;
                              friend class MotionSensor<SinricProMotionsensor>;
  public:
    SinricProMotionsensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "MOTION_SENSOR") {}
};

#endif

