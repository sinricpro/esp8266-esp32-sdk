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
#include "Capabilities/RangeController.h"
#include "Capabilities/PowerStateController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

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
                        public PushNotification<SinricProBlinds>,
                        public PowerStateController<SinricProBlinds>,
                        public RangeController<SinricProBlinds> {
                        friend class SettingController<SinricProBlinds>;
                        friend class PushNotification<SinricProBlinds>;
                        friend class PowerStateController<SinricProBlinds>;
                        friend class RangeController<SinricProBlinds>;
  public:
    SinricProBlinds(const String &deviceId) : SinricProDevice(deviceId, "BLIND"){}
};

} // SINRICPRO_NAMESPACE

using SinricProBlinds = SINRICPRO_NAMESPACE::SinricProBlinds;