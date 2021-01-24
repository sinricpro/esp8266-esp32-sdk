/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICWINDOWAC_H_
#define _SINRICWINDOWAC_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/RangeController.h"
#include "./Controller/ThermostatController.h"

/**
 * @class SinricProWindowAC
 * @brief Device to control Window Air Conditioner 
 * 
 * Support
 * * Set / adjust target temperature
 * * Set mode `AUTO`, `COOL`, `HEAT`
 * * Set range value 
 * * Report target temperature
 * * Report actual temperature
 **/

class SinricProWindowAC :  public SinricProDevice,
                           public PowerStateController,
                           public RangeController,
                           public ThermostatController {
  public:
	  SinricProWindowAC(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;

};

SinricProWindowAC::SinricProWindowAC(const DeviceId &deviceId) : SinricProDevice(deviceId, "AC_UNIT"),
                                                                 PowerStateController(this),
                                                                 RangeController(this),
                                                                 ThermostatController(this) {}

bool SinricProWindowAC::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  if (!success) success = RangeController::handleRequest(action, request_value, response_value);
  if (!success) success = ThermostatController::handleRequest(action, request_value, response_value);

  return success;
}


#endif