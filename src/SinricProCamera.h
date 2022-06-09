
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
#include "Capabilities/PowerStateController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProCamera
 * @brief Camera suporting basic on / off command
 * @ingroup Devices
 **/
class SinricProCamera : public SinricProDevice,
                        public SettingController<SinricProCamera>,
                        public PushNotification<SinricProCamera>,
                        public PowerStateController<SinricProCamera> {
                        friend class SettingController<SinricProCamera>;
                        friend class PushNotification<SinricProCamera>;
                        friend class PowerStateController<SinricProCamera>;
  public:
	  SinricProCamera(const String &deviceId) : SinricProDevice(deviceId, "CAMERA") {}
};

} // SINRICPRO_NAMESPACE

using SinricProCamera = SINRICPRO_NAMESPACE::SinricProCamera;