
/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICCAMERA_H_
#define _SINRICCAMERA_H_

#include "SinricProDevice.h"

/**
 * @class SinricProCamera
 * @brief Camera suporting basic on / off command
 **/
class SinricProCamera :  public SinricProDevice {
  public:
	  SinricProCamera(const DeviceId &deviceId);
    String getProductType() { return SinricProDevice::getProductType() + String("CAMERA"); }    
};

SinricProCamera::SinricProCamera(const DeviceId &deviceId) : SinricProDevice(deviceId) {}


#endif

