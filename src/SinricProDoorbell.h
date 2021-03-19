/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDOORBELL_H_
#define _SINRICDOORBELL_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/Doorbell.h"

/**
 * @class SinricProDoorbell
 * @brief Device to report doorbell events
 * @ingroup Devices
 **/
class SinricProDoorbell :  public SinricProDevice,
                           public SettingController<SinricProDoorbell>,
                           public PowerStateController<SinricProDoorbell>,
                           public Doorbell<SinricProDoorbell> {
                           friend class SettingController<SinricProDoorbell>;
                           friend class PowerStateController<SinricProDoorbell>;
                           friend class Doorbell<SinricProDoorbell>;
  public:
	  SinricProDoorbell(const DeviceId &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR") {}
};

#endif

