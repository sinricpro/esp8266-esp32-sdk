/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDIMSWITCH_H_
#define _SINRICDIMSWITCH_H_

#include "SinricProDevice.h"
#include "./Controller/PowerstateController.h"
#include "./Controller/PowerLevelController.h"

/**
 * @class SinricProDimSwitch
 * @brief Device which supports on / off and dimming commands
 * @ingroup Devices
 **/
class SinricProDimSwitch :  public SinricProDevice,
                            public PowerStateController<SinricProDimSwitch>,
                            public PowerLevelController<SinricProDimSwitch> {
                            friend class PowerStateController<SinricProDimSwitch>;
                            friend class PowerLevelController<SinricProDimSwitch>;
  public:
	  SinricProDimSwitch(const DeviceId &deviceId);
  protected:
    bool handleRequest(const String &action, const String& instance, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProDimSwitch::SinricProDimSwitch(const DeviceId &deviceId) : SinricProDevice(deviceId, "DIMMABLE_SWITCH") {
}

bool SinricProDimSwitch::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  (void) instance;
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handlePowerLevelController(action, request_value, response_value)) return true;

  return false;
}

#endif

