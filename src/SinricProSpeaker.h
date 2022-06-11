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
#include "Capabilities/MuteController.h"
#include "Capabilities/VolumeController.h"
#include "Capabilities/MediaController.h"
#include "Capabilities/InputController.h"
#include "Capabilities/EqualizerController.h"
#include "Capabilities/ModeController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

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
                         public SettingController<SinricProSpeaker>,
                         public PushNotification<SinricProSpeaker>,
                         public PowerStateController<SinricProSpeaker>,
                         public MuteController<SinricProSpeaker>,
                         public VolumeController<SinricProSpeaker>,
                         public MediaController<SinricProSpeaker>,
                         public InputController<SinricProSpeaker>,
                         public EqualizerController<SinricProSpeaker>,
                         public ModeController<SinricProSpeaker> {
                         friend class SettingController<SinricProSpeaker>;
                         friend class PushNotification<SinricProSpeaker>;
                         friend class PowerStateController<SinricProSpeaker>;
                         friend class MuteController<SinricProSpeaker>;
                         friend class VolumeController<SinricProSpeaker>;
                         friend class MediaController<SinricProSpeaker>;
                         friend class InputController<SinricProSpeaker>;
                         friend class EqualizerController<SinricProSpeaker>;
                         friend class ModeController<SinricProSpeaker>;
public:
  SinricProSpeaker(const String &deviceId) : SinricProDevice(deviceId, "SPEAKER") {}
};

} // SINRICPRO_NAMESPACE

using SinricProSpeaker = SINRICPRO_NAMESPACE::SinricProSpeaker;