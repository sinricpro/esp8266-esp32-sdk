/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDIMSWITCH_H_
#define _SINRICDIMSWITCH_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerstateController.h"
#include "Capabilities/PowerLevelController.h"

/**
 * @class SinricProDimSwitch
 * @brief Device which supports on / off and dimming commands
 * @ingroup Devices
 **/
class SinricProDimSwitch :  public SinricProDevice,
                            public SettingController<SinricProDimSwitch>,
                            public PowerStateController<SinricProDimSwitch>,
                            public PowerLevelController<SinricProDimSwitch> {
                            friend class SettingController<SinricProDimSwitch>;
                            friend class PowerStateController<SinricProDimSwitch>;
                            friend class PowerLevelController<SinricProDimSwitch>;
  public:
    SinricProDimSwitch(const DeviceId &deviceId) : SinricProDevice(deviceId, "DIMMABLE_SWITCH"){};
};

#endif

