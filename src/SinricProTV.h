/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTV_H_
#define _SINRICTV_H_

#include "SinricProDevice.h"
#include "./Controller/PowerStateController.h"
#include "./Controller/VolumeController.h"
#include "./Controller/MuteController.h"
#include "./Controller/MediaController.h"
#include "./Controller/InputController.h"
#include "./Controller/ChannelController.h"

/**
 * @class SinricProTV
 * @brief Device to control a TV
 * 
 * Supporting: 
 * * setVolume / adjustVolume
 * * mute / unmute
 * * media controls:
 *   * `FastForward`, `Next`, `Pause`, `Play`, `Previous`, `Rewind`, `StartOver`, `Stop`
 * * select input 
 *   * `AUX 1`..`AUX 7`, `BLURAY`, `CABLE`, `CD`, `COAX 1`,`COAX 2`, `COMPOSITE 1`, `DVD`, `GAME`, `HD RADIO`, `HDMI 1`.. `HDMI 10`, `HDMI ARC`, `INPUT 1`..`INPUT 10`, `IPOD`, `LINE 1`..`LINE 7`, `MEDIA PLAYER`, `OPTICAL 1`, `OPTICAL 2`, `PHONO`, `PLAYSTATION`, `PLAYSTATION 3`, `PLAYSTATION 4`, `SATELLITE`, `SMARTCAST`, `TUNER`, `TV`, `USB DAC`, `VIDEO 1`..`VIDEO 3`, `XBOX`
 * * Change channel by number
 * * Change channel by name
 * * Skip channels
 */
class SinricProTV : public SinricProDevice,
                    public PowerStateController,
                    public VolumeController,
                    public MuteController,
                    public MediaController,
                    public InputController,
                    public ChannelController {
  public:
	  SinricProTV(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProTV::SinricProTV(const DeviceId &deviceId) : SinricProDevice(deviceId, "TV"),
                                                     PowerStateController(this),
                                                     VolumeController(this),
                                                     MuteController(this),
                                                     MediaController(this),
                                                     InputController(this),
                                                     ChannelController(this) {}

bool SinricProTV::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;

  if (!success) success = PowerStateController::handleRequest(action, request_value, response_value);
  if (!success) success = VolumeController::handleRequest(action, request_value, response_value);
  if (!success) success = MuteController::handleRequest(action, request_value, response_value);
  if (!success) success = MediaController::handleRequest(action, request_value, response_value);
  if (!success) success = InputController::handleRequest(action, request_value, response_value);
  if (!success) success = ChannelController::handleRequest(action, request_value, response_value);

  return success;
}

#endif

