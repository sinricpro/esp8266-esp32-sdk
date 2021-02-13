/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICCONTACTSENSOR_H_
#define _SINRICCONTACTSENSOR_H_

#include "SinricProDevice.h"
#include "Controller/PowerStateController.h"
#include "EventSource/ContactEventSource.h"

/**
 * @class SinricProContactsensor
 * @brief Device to report contact sensor events
 * @ingroup Devices
 **/
class SinricProContactsensor : public SinricProDevice,
                               public PowerStateController<SinricProContactsensor>,
                               public ContactEventSource<SinricProContactsensor> {
                               friend class PowerStateController<SinricProContactsensor>;
                               friend class ContactEventSource<SinricProContactsensor>;
  public:
	  SinricProContactsensor(const DeviceId &deviceId);
  protected:
    bool handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProContactsensor::SinricProContactsensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR") {}

bool SinricProContactsensor::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  (void) instance;
  if (handlePowerStateController(action, request_value, response_value)) return true;
  return false;
}

#endif

