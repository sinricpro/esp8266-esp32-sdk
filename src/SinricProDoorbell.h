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
#include "Capabilities/Doorbell.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProDoorbell
 * @brief Device to report doorbell events
 * @ingroup Devices
 **/
class SinricProDoorbell :  public SinricProDevice,
                           public SettingController<SinricProDoorbell>,
                           public PushNotification<SinricProDoorbell>,
                           public PowerStateController<SinricProDoorbell>,
                           public Doorbell<SinricProDoorbell> {
                           friend class SettingController<SinricProDoorbell>;
                           friend class PushNotification<SinricProDoorbell>;
                           friend class PowerStateController<SinricProDoorbell>;
                           friend class Doorbell<SinricProDoorbell>;
  public:
	  SinricProDoorbell(const String &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR") {}
};

} // Namespace

using SinricProDoorbell = SINRICPRO_NAMESPACE::SinricProDoorbell;