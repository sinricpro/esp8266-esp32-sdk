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
 * @ingroup Devices
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
                    public PowerStateController<SinricProTV>,
                    public VolumeController<SinricProTV>,
                    public MuteController<SinricProTV>,
                    public MediaController<SinricProTV>,
                    public InputController<SinricProTV>,
                    public ChannelController<SinricProTV> {
  public:
	  SinricProTV(const DeviceId &deviceId);

    bool handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) override;
};

SinricProTV::SinricProTV(const DeviceId &deviceId) : SinricProDevice(deviceId, "TV") {}

bool SinricProTV::handleRequest(const DeviceId &deviceId, const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handleVolumeController(action, request_value, response_value)) return true;
  if (handleMuteController(action, request_value, response_value)) return true;
  if (handleMediaController(action, request_value, response_value)) return true;
  if (handleInputController(action, request_value, response_value)) return true;
  if (handleChannelController(action, request_value, response_value)) return true;

  return false;
}

#endif

