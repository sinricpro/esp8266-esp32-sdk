/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICBLINDS_H_
#define _SINRICBLINDS_H_

#include "SinricProDevice.h"
#include "Controller/RangeController.h"
#include "Controller/PowerStateController.h"

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
                        public PowerStateController<SinricProBlinds>,
                        public RangeController<SinricProBlinds> {
                        friend class PowerStateController<SinricProBlinds>;
                        friend class RangeController<SinricProBlinds>;
  public:
	  SinricProBlinds(const DeviceId &deviceId);

    bool handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) override;

  private:
};

SinricProBlinds::SinricProBlinds(const DeviceId &deviceId) : SinricProDevice(deviceId, "BLIND") {}

bool SinricProBlinds::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handleRangeController(action, instance, request_value, response_value)) return true;
  return false;
}

#endif

