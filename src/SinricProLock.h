/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLOCK_H_
#define _SINRICLOCK_H_

#include "SinricProDevice.h"
#include "Capabilities/LockController.h"

/**
 * @class SinricProLock
 * @brief Device to control a smart lock
 * @ingroup Devices
 * 
 * Supporting 
 * * on / off
 * * lock / unlock
 **/
class SinricProLock :  public SinricProDevice,
                       public LockController<SinricProLock> {
                       friend class LockController<SinricProLock>;
  public:
	  SinricProLock(const DeviceId &deviceId) : SinricProDevice(deviceId, "SMARTLOCK") {}
};

#endif

