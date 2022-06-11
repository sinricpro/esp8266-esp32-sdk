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
#include "Capabilities/DoorController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

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
                            public PushNotification<SinricProGarageDoor>,
                            public DoorController<SinricProGarageDoor> {
                            friend class SettingController<SinricProGarageDoor>;
                            friend class PushNotification<SinricProGarageDoor>;
                            friend class DoorController<SinricProGarageDoor>;
  public:
	  SinricProGarageDoor(const String &deviceId) : SinricProDevice(deviceId, "GARAGE_DOOR") {}
};

} // Namespace

using SinricProGarageDoor = SINRICPRO_NAMESPACE::SinricProGarageDoor;