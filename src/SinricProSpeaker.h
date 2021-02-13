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
#include "Controller/EqualizerController.h"
#include "Controller/ModeController.h"

/**
 * @class SinricProSpeaker
 * @brief Device to control a smart speaker
 * @ingroup Devices
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
                         public PowerStateController<SinricProSpeaker>,
                         public MuteController<SinricProSpeaker>,
                         public VolumeController<SinricProSpeaker>,
                         public MediaController<SinricProSpeaker>,
                         public InputController<SinricProSpeaker>,
                         public EqualizerController<SinricProSpeaker>,
                         public ModeController<SinricProSpeaker> {
                         friend class PowerStateController<SinricProSpeaker>;
                         friend class MuteController<SinricProSpeaker>;
                         friend class VolumeController<SinricProSpeaker>;
                         friend class MediaController<SinricProSpeaker>;
                         friend class InputController<SinricProSpeaker>;
                         friend class EqualizerController<SinricProSpeaker>;
                         friend class ModeController<SinricProSpeaker>;
public:
  SinricProSpeaker(const DeviceId &deviceId);
protected:
  bool handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
};

SinricProSpeaker::SinricProSpeaker(const DeviceId &deviceId) : SinricProDevice(deviceId, "SPEAKER") {}

bool SinricProSpeaker::handleRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value) {
  if (handlePowerStateController(action, request_value, response_value)) return true;
  if (handleMuteController(action, request_value, response_value)) return true;
  if (handleVolumeController(action, request_value, response_value)) return true;
  if (handleMediaController(action, request_value, response_value)) return true;
  if (handleInputController(action, request_value, response_value)) return true;
  if (handleEqualizerController(action, request_value, response_value)) return true;
  if (handleModeController(action, instance, request_value, response_value)) return true;

  return false;
}

#endif

