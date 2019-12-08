/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICFAN_H_
#define _SINRICFAN_H_

#include "SinricProDevice.h"
#include "SinricProDimSwitch.h"

/**
 * @class SinricProFan
 * @brief Device to turn on / off a fan and change it's speed by using powerlevel
 **/
class SinricProFan :  public SinricProDimSwitch {
  public:
	  SinricProFan(const char* deviceId, unsigned long eventWaitTime=100);
};

SinricProFan::SinricProFan(const char* deviceId, unsigned long eventWaitTime) : SinricProDimSwitch(deviceId, eventWaitTime) {}

#endif

