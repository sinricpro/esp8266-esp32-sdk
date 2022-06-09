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
#include "Capabilities/ContactSensor.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProContactsensor
 * @brief Device to report contact sensor events
 * @ingroup Devices
 **/
class SinricProContactsensor : public SinricProDevice,
                               public SettingController<SinricProContactsensor>,
                               public PushNotification<SinricProContactsensor>,
                               public PowerStateController<SinricProContactsensor>,
                               public ContactSensor<SinricProContactsensor> {
                               friend class SettingController<SinricProContactsensor>;
                               friend class PushNotification<SinricProContactsensor>;
                               friend class PowerStateController<SinricProContactsensor>;
                               friend class ContactSensor<SinricProContactsensor>;
  public:
	  SinricProContactsensor(const String &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR") {}
};

} // SINRICPRO_NAMESPACE

using SinricProContactsensor = SINRICPRO_NAMESPACE::SinricProContactsensor;