/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICSWITCH_H_
#define _SINRICSWITCH_H_

#include "SinricProDevice.h"

/**
 * @class SinricProSwitch
 * @brief Device suporting basic on / off command
 **/
class SinricProSwitch :  public SinricProDevice {
  public:
	  SinricProSwitch(const char* deviceId, unsigned long eventWaitTime=100);
};

SinricProSwitch::SinricProSwitch(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime) {}


#endif

