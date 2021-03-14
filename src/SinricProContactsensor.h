/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICCONTACTSENSOR_H_
#define _SINRICCONTACTSENSOR_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/ContactSensor.h"

/**
 * @class SinricProContactsensor
 * @brief Device to report contact sensor events
 * @ingroup Devices
 **/
class SinricProContactsensor : public SinricProDevice,
                               public SettingController<SinricProContactsensor>,
                               public PowerStateController<SinricProContactsensor>,
                               public ContactSensor<SinricProContactsensor> {
                               friend class SettingController<SinricProContactsensor>;
                               friend class PowerStateController<SinricProContactsensor>;
                               friend class ContactSensor<SinricProContactsensor>;
  public:
	  SinricProContactsensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR") {}
};

#endif

