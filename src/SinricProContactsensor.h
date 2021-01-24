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
#include "Controller/ContactController.h"

/**
 * @class SinricProContactsensor
 * @brief Device to report contact sensor events
 **/
class SinricProContactsensor : public SinricProDevice,
                               public PowerStateController,
                               public ContactController {
  public:
	  SinricProContactsensor(const DeviceId &deviceId);
    bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
};

SinricProContactsensor::SinricProContactsensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "CONTACT_SENSOR"),
                                                                           PowerStateController(this),
                                                                           ContactController(this) {}

bool SinricProContactsensor::handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

