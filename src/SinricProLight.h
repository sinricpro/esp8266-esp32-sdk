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
 * 
 * Supporting 
 * * On / Off
 * * Brightness (0..100)
 * * Color (RGB)
 * * Color temperature
 **/
class SinricProLight :  public SinricProDevice,
                        public PowerStateController,
                        public BrightnessController,
                        public ColorController,
                        public ColorTemperatureController {
  public:
    SinricProLight(const DeviceId &deviceId);
    String getProductType() { return SinricProDevice::getProductType() + String("LIGHT"); }
    // callback


  


    // event
    // handle
    bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value);
  private:
    ColorTemperatureCallback colorTemperatureCallback;
    IncreaseColorTemperatureCallback increaseColorTemperatureCallback;
    DecreaseColorTemperatureCallback decreaseColorTemperatureCallback;
};

SinricProLight::SinricProLight(const DeviceId &deviceId) : 
  SinricProDevice(deviceId),
  PowerStateController(this),
  BrightnessController(this),
  ColorController(this),
  ColorTemperatureController(this) {}

bool SinricProLight::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (deviceId != this->deviceId) return false;

  bool success = false;
  if (!success) BrightnessController::handleRequest(action, request_value, response_value);
  if (!success) ColorController::handleRequest(action, request_value, response_value);
  if (!success) ColorTemperatureController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

