/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICMOTIONSENSOR_H_
#define _SINRICMOTIONSENSOR_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./EventSource/MotionEventSource.h"

/**
 * @class SinricProMotionsensor
 * @brief Device to report motion detection events
 * @ingroup Devices
 */
class SinricProMotionsensor : public SinricProDevice,
                              public PowerStateController<SinricProMotionsensor>,
                              public MotionEventSource<SinricProMotionsensor> {
                              friend class PowerStateController<SinricProMotionsensor>;
                              friend class MotionEventSource<SinricProMotionsensor>;
  public:
    SinricProMotionsensor(const DeviceId &deviceId);
  protected:
    bool handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProMotionsensor::SinricProMotionsensor(const DeviceId &deviceId) : SinricProDevice(deviceId, "MOTION_SENSOR") {}

bool SinricProMotionsensor::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  (void) instance;
  if (handlePowerStateController(action, request_value, response_value)) return true;
  return false;
}

#endif

