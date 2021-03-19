/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICFAN_H_
#define _SINRICFAN_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/PowerLevelController.h"

/**
 * @class SinricProFan
 * @brief Device to turn on / off a fan and change it's speed by using powerlevel
 * @ingroup Devices
 **/
class SinricProFan : public SinricProDevice,
                     public SettingController<SinricProFan>,
                     public PowerStateController<SinricProFan>,
                     public PowerLevelController<SinricProFan> {
                     friend class SettingController<SinricProFan>;
                     friend class PowerStateController<SinricProFan>;
                     friend class PowerLevelController<SinricProFan>;
  public:
	  SinricProFan(const DeviceId &deviceId) : SinricProDevice(deviceId, "FAN_NON-US") {}
};

#endif

