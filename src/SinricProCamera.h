
/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICCAMERA_H_
#define _SINRICCAMERA_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"

/**
 * @class SinricProCamera
 * @brief Camera suporting basic on / off command
 * @ingroup Devices
 **/
class SinricProCamera : public SinricProDevice,
                        public SettingController<SinricProCamera>,
                        public PowerStateController<SinricProCamera> {
                        friend class SettingController<SinricProCamera>;
                        friend class PowerStateController<SinricProCamera>;
  public:
	  SinricProCamera(const DeviceId &deviceId) : SinricProDevice(deviceId, "CAMERA") {}
};

#endif

