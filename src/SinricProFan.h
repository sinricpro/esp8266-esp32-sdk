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
#include "Capabilities/PowerLevelController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProFan
 * @brief Device to turn on / off a fan and change it's speed by using powerlevel
 * @ingroup Devices
 **/
class SinricProFan : public SinricProDevice,
                     public SettingController<SinricProFan>,
                     public PushNotification<SinricProFan>,
                     public PowerStateController<SinricProFan>,
                     public PowerLevelController<SinricProFan> {
                     friend class SettingController<SinricProFan>;
                     friend class PushNotification<SinricProFan>;
                     friend class PowerStateController<SinricProFan>;
                     friend class PowerLevelController<SinricProFan>;
  public:
	  SinricProFan(const String &deviceId) : SinricProDevice(deviceId, "FAN_NON-US") {}
};

} // Namespace

using SinricProFan = SINRICPRO_NAMESPACE::SinricProFan;