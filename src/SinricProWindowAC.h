/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICWINDOWAC_H_
#define _SINRICWINDOWAC_H_

#include "SinricProDevice.h"
#include "Capabilities/PowerStateController.h"
#include "Capabilities/RangeController.h"
#include "Capabilities/ThermostatController.h"

/**
 * @class SinricProWindowAC
 * @brief Device to control Window Air Conditioner 
 * @ingroup Devices
 * 
 * Support
 * * Set / adjust target temperature
 * * Set mode `AUTO`, `COOL`, `HEAT`
 * * Set range value 
 * * Report target temperature
 * * Report actual temperature
 **/

class SinricProWindowAC :  public SinricProDevice,
                           public PowerStateController<SinricProWindowAC>,
                           public RangeController<SinricProWindowAC>,
                           public ThermostatController<SinricProWindowAC> {
                           friend class public PowerStateController<SinricProWindowAC>;
                           friend class public RangeController<SinricProWindowAC>;
                           friend class public ThermostatController<SinricProWindowAC>;
  public:
	  SinricProWindowAC(const DeviceId &deviceId) : SinricProDevice(deviceId, "AC_UNIT") {}
};

#endif