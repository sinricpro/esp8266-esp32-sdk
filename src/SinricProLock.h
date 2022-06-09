/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/PushNotification.h"
#include "Capabilities/LockController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

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
                       public SettingController<SinricProLock>,
                       public PushNotification<SinricProLock>,
                       public LockController<SinricProLock> {
                       friend class SettingController<SinricProLock>;
                       friend class PushNotification<SinricProLock>;
                       friend class LockController<SinricProLock>;
  public:
	  SinricProLock(const String &deviceId) : SinricProDevice(deviceId, "SMARTLOCK") {}
};

} // SINRICPRO_NAMESPACE#

using SinricProLock = SINRICPRO_NAMESPACE::SinricProLock;