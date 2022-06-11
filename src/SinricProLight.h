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
#include "Capabilities/BrightnessController.h"
#include "Capabilities/ColorController.h"
#include "Capabilities/ColorTemperatureController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @class SinricProLight
 * @brief Device to control a light 
 * @ingroup Devices
 * 
 * Supporting 
 * * On / Off
 * * Brightness (0..100)
 * * Color (RGB)
 * * Color temperature
 **/
class SinricProLight :  public SinricProDevice,
                        public SettingController<SinricProLight>,
                        public PushNotification<SinricProLight>,
                        public PowerStateController<SinricProLight>,
                        public BrightnessController<SinricProLight>,
                        public ColorController<SinricProLight>,
                        public ColorTemperatureController<SinricProLight> {
                        friend class SettingController<SinricProLight>;
                        friend class PushNotification<SinricProLight>;
                        friend class PowerStateController<SinricProLight>;
                        friend class BrightnessController<SinricProLight>;
                        friend class ColorController<SinricProLight>;
                        friend class ColorTemperatureController<SinricProLight>;
  public:
    SinricProLight(const String &deviceId) : SinricProDevice(deviceId, "LIGHT") {}
};

} // SINRICPRO_NAMESPACE

using SinricProLight = SINRICPRO_NAMESPACE::SinricProLight;