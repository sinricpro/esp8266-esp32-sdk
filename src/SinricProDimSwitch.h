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
 * @class SinricProDimSwitch
 * @brief Device which supports on / off and dimming commands
 * @ingroup Devices
 **/
class SinricProDimSwitch :  public SinricProDevice,
                            public SettingController<SinricProDimSwitch>,
                            public PushNotification<SinricProDimSwitch>,
                            public PowerStateController<SinricProDimSwitch>,
                            public PowerLevelController<SinricProDimSwitch> {
                            friend class SettingController<SinricProDimSwitch>;
                            friend class PushNotification<SinricProDimSwitch>;
                            friend class PowerStateController<SinricProDimSwitch>;
                            friend class PowerLevelController<SinricProDimSwitch>;
  public:
    SinricProDimSwitch(const String &deviceId) : SinricProDevice(deviceId, "DIMMABLE_SWITCH"){};
};

} // SINRICPRO_NAMESPACE

using SinricProDimSwitch = SINRICPRO_NAMESPACE::SinricProDimSwitch;