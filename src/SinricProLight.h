/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLIGHT_H_
#define _SINRICLIGHT_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/BrightnessController.h"
#include "./Controller/ColorController.h"
#include "./Controller/ColorTemperatureController.h"

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
                        public PowerStateController<SinricProLight>,
                        public BrightnessController<SinricProLight>,
                        public ColorController<SinricProLight>,
                        public ColorTemperatureController<SinricProLight> {
  public:
    SinricProLight(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProLight::SinricProLight(const DeviceId &deviceId) : SinricProDevice(deviceId, "LIGHT") {}

bool SinricProLight::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handleBrightnessController(action, request_value, response_value)) return true;
  if (handleColorController(action, request_value, response_value)) return true;
  if (handleColorTemperatureController(action, request_value, response_value)) return true;

  return false;
}

#endif

