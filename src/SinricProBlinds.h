/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICBLINDS_H_
#define _SINRICBLINDS_H_

#include "SinricProDevice.h"
#include "Capabilities/SettingController.h"
#include "Capabilities/RangeController.h"
#include "Capabilities/PowerStateController.h"

/**
 * @class SinricProBlinds
 * @brief Device to control interior blinds
 * @ingroup Devices
 * 
 * Supporting 
 * * On / Off
 * * Position (0..100)
 * * open / close 
 **/
class SinricProBlinds : public SinricProDevice,
                        public SettingController<SinricProBlinds>,
                        public PowerStateController<SinricProBlinds>,
                        public RangeController<SinricProBlinds> {
                        friend class SettingController<SinricProBlinds>;
                        friend class PowerStateController<SinricProBlinds>;
                        friend class RangeController<SinricProBlinds>;
  public:
    SinricProBlinds(const DeviceId &deviceId) : SinricProDevice(deviceId, "BLIND"){};
};

#endif

