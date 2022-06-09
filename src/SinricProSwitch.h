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

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProSwitch
 * @brief Device suporting basic on / off command
 * @ingroup Devices
 **/
class SinricProSwitch : public SinricProDevice,
                        public SettingController<SinricProSwitch>,
                        public PushNotification<SinricProSwitch>,
                        public PowerStateController<SinricProSwitch> {
                        friend class SettingController<SinricProSwitch>;
                        friend class PushNotification<SinricProSwitch>;
                        friend class PowerStateController<SinricProSwitch>;
  public:
    SinricProSwitch(const String &deviceId) : SinricProDevice(deviceId, "SWITCH") {};
};

} // SINRICPRO_NAMESPACE

using SinricProSwitch = SINRICPRO_NAMESPACE::SinricProSwitch;