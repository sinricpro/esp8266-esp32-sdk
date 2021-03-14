/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICGARAGEDOOR_H_
#define _SINRICGARAGEDOOR_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/DoorController.h"

/**
 * @class SinricProGarageDoor
 * @brief Device to control a garage door
 * @ingroup Devices
 * 
 * Supporting 
 * * open / close 
 **/
class SinricProGarageDoor : public SinricProDevice,
                            public SettingController<SinricProGarageDoor>,
                            public DoorController<SinricProGarageDoor> {
                            friend class SettingController<SinricProGarageDoor>;
                            friend class DoorController<SinricProGarageDoor>;
  public:
	  SinricProGarageDoor(const DeviceId &deviceId) : SinricProDevice(deviceId, "GARAGE_DOOR") {}
};

#endif