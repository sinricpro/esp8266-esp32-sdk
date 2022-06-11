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
#include "Capabilities/RangeController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProFanUS
 * @brief Device to control a fan with on / off commands and its speed by a range value
 * @ingroup Devices
 */
class SinricProFanUS :  public SinricProDevice,
                        public SettingController<SinricProFanUS>,
                        public PushNotification<SinricProFanUS>,
                        public PowerStateController<SinricProFanUS>,
                        public RangeController<SinricProFanUS> {
                        friend class SettingController<SinricProFanUS>;
                        friend class PushNotification<SinricProFanUS>;
                        friend class PowerStateController<SinricProFanUS>;
                        friend class RangeController<SinricProFanUS>;
  public:
	  SinricProFanUS(const String &deviceId) : SinricProDevice(deviceId, "FAN") {}
};

} // Namespace

using SinricProFanUS = SINRICPRO_NAMESPACE::SinricProFanUS;