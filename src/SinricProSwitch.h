/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICSWITCH_H_
#define _SINRICSWITCH_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"

/**
 * @class SinricProSwitch
 * @brief Device suporting basic on / off command
 * @ingroup Devices
 **/
class SinricProSwitch : public SinricProDevice,
                        public SettingController<SinricProSwitch>,
                        public PowerStateController<SinricProSwitch> {
                        friend class SettingController<SinricProSwitch>;
                        friend class PowerStateController<SinricProSwitch>;
  public:
    SinricProSwitch(const DeviceId &deviceId) : SinricProDevice(deviceId, "SWITCH") {};
};

#endif

