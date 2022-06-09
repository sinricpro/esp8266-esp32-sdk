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
#include "Capabilities/VolumeController.h"
#include "Capabilities/MuteController.h"
#include "Capabilities/MediaController.h"
#include "Capabilities/InputController.h"
#include "Capabilities/ChannelController.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

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
                    public SettingController<SinricProTV>,
                    public PushNotification<SinricProTV>,
                    public PowerStateController<SinricProTV>,
                    public VolumeController<SinricProTV>,
                    public MuteController<SinricProTV>,
                    public MediaController<SinricProTV>,
                    public InputController<SinricProTV>,
                    public ChannelController<SinricProTV> {
                    friend class SettingController<SinricProTV>;
                    friend class PushNotification<SinricProTV>;
                    friend class PowerStateController<SinricProTV>;
                    friend class VolumeController<SinricProTV>;
                    friend class MuteController<SinricProTV>;
                    friend class MediaController<SinricProTV>;
                    friend class InputController<SinricProTV>;
                    friend class ChannelController<SinricProTV>;
  public:
	  SinricProTV(const String &deviceId) : SinricProDevice(deviceId, "TV") {}
};

} // SINRICPRO_NAMESPACE

using SinricProTV = SINRICPRO_NAMESPACE::SinricProTV;