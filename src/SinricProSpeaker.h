/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICSPEAKER_H_
#define _SINRICSPEAKER_H_

#include "SinricProDevice.h"
#include "Controller/PowerStateController.h"
#include "Controller/MuteController.h"
#include "Controller/VolumeController.h"
#include "Controller/MediaController.h"
#include "Controller/InputController.h"
#include "Controller/BandsController.h"
#include "Controller/ModeController.h"

/**
 * @class SinricProSpeaker
 * @brief Device to control a smart speaker
 * 
 * Supporting: 
 * * setVolume / adjustVolume
 * * set / adjust / reset bands (BASS, MIDRANGE, TREBBLE)
 * * mute / unmute
 * * media controls:
 *   * FastForward
 *   * Next
 *   * Pause
 *   * Play
 *   * Previous
 *   * Rewind
 *   * StartOver
 *   * Stop
 * * set mode (TV, MOVIE, ...)
 */
class SinricProSpeaker : public SinricProDevice,
                         public PowerStateController,
                         public MuteController,
                         public VolumeController,
                         public MediaController,
                         public InputController,
                         public BandsController,
                         public ModeController {
public:
  SinricProSpeaker(const DeviceId &deviceId);
  bool handleRequest(const DeviceId &deviceId, const char *action, JsonObject &request_value, JsonObject &response_value);

private:
};

SinricProSpeaker::SinricProSpeaker(const DeviceId &deviceId) : SinricProDevice(deviceId, "SPEAKER"),
                                                               PowerStateController(this),
                                                               MuteController(this),
                                                               VolumeController(this),
                                                               MediaController(this),
                                                               InputController(this),
                                                               BandsController(this),
                                                               ModeController(this) {}

bool SinricProSpeaker::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  if (!success) success = MuteController::handleRequest(action, request_value, response_value);
  if (!success) success = VolumeController::handleRequest(action, request_value, response_value);
  if (!success) success = MediaController::handleRequest(action, request_value, response_value);
  if (!success) success = InputController::handleRequest(action, request_value, response_value);
  if (!success) success = BandsController::handleRequest(action, request_value, response_value);
  if (!success) success = ModeController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

