/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICTV_H_
#define _SINRICTV_H_

#include "SinricProDevice.h"

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
class SinricProTV :  public SinricProDevice {
  public:
	  SinricProTV(const char* deviceId, unsigned long eventWaitTime=100);
    // callback

    /**
     * @brief Callback definition for onSetVolume function
     * 
     * Gets called when device receive a `setVolume` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   volume      Integer with volume device should set to
     * @param[out]  volume      Integer with volume device has been set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SetVolumeCallback_TV Example-Code
     * @snippet callbacks.cpp onSetVolume
     **/
    typedef std::function<bool(const String&, int&)> SetVolumeCallback;
    
    /**
     * @brief Callback definition for onAdjustVolume function
     * 
     * Gets called when device receive a `adjustVolume` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   volumeDelta Integer with relative volume the device should change about (-100..100)
     * @param[out]  volumeDelta Integer with absolute volume device has been set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustVolumeCallback_TV Example-Code
     * @snippet callbacks.cpp onAdjustVolume
     **/      
    typedef std::function<bool(const String&, int&)> AdjustVolumeCallback;

    /**
     * @brief Callback definition for onMute function
     * 
     * Gets called when device receive a `setMute` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   mute        `true` mute device \n `false` unmute device
     * @param[out]  mute        `true` device is muted \n `false` device is unmuted
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section MuteCallback_TV Example-Code
     * @snippet callbacks.cpp onMute
     **/  
    typedef std::function<bool(const String&, bool&)> MuteCallback;

    /**
     * @brief Callback definition for onMediaControl function
     * 
     * Gets called when device receive a `mediaControl` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   control     String with requested control \n `FastForward`, `Next`, `Pause`, `Play`, `Previous`, `Rewind`, `StartOver`, `Stop`
     * @param[out]  control     String with control \n `FastForward`, `Next`, `Pause`, `Play`, `Previous`, `Rewind`, `StartOver`, `Stop`
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section MediaControlCallback_TV Example-Code
     * @snippet callbacks.cpp onMediaControl
     **/
    typedef std::function<bool(const String&, String&)> MediaControlCallback;

    /**
     * @brief Callback definition for onSelectInput function
     * 
     * Gets called when device receive a `selectInput` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   input       String with input name device is requested to switch to \n `AUX 1`..`AUX 7`, `BLURAY`, `CABLE`, `CD`, `COAX 1`,`COAX 2`, `COMPOSITE 1`, `DVD`, `GAME`, `HD RADIO`, `HDMI 1`.. `HDMI 10`, `HDMI ARC`, `INPUT 1`..`INPUT 10`, `IPOD`, `LINE 1`..`LINE 7`, `MEDIA PLAYER`, `OPTICAL 1`, `OPTICAL 2`, `PHONO`, `PLAYSTATION`, `PLAYSTATION 3`, `PLAYSTATION 4`, `SATELLITE`, `SMARTCAST`, `TUNER`, `TV`, `USB DAC`, `VIDEO 1`..`VIDEO 3`, `XBOX`
     * @param[out]  input       String with input name device has switchted to \n `AUX 1`..`AUX 7`, `BLURAY`, `CABLE`, `CD`, `COAX 1`,`COAX 2`, `COMPOSITE 1`, `DVD`, `GAME`, `HD RADIO`, `HDMI 1`.. `HDMI 10`, `HDMI ARC`, `INPUT 1`..`INPUT 10`, `IPOD`, `LINE 1`..`LINE 7`, `MEDIA PLAYER`, `OPTICAL 1`, `OPTICAL 2`, `PHONO`, `PLAYSTATION`, `PLAYSTATION 3`, `PLAYSTATION 4`, `SATELLITE`, `SMARTCAST`, `TUNER`, `TV`, `USB DAC`, `VIDEO 1`..`VIDEO 3`, `XBOX`
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SelectInput Example-Code
     * @snippet callbacks.cpp onSelectInput
     **/
    typedef std::function<bool(const String&, String&)> SelectInputCallback;

    /**
     * @brief Callback definition for onChangeChannel function
     * 
     * Gets called when device receive a `changeChannel` request by using channel name
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   channel      String with channel name device is requested to switch to
     * @param[out]  channel      String with channel name device has switchted to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section ChangeChannel Example-Code
     * @snippet callbacks.cpp onChangeChannel
     **/
    typedef std::function<bool(const String&, String&)> ChangeChannelCallback;

    /**
     * @brief Callback definition for onChangeChannelNumber function
     * 
     * Gets called when device receive a `changeChannel` request by using channel number 
     * @param[in]   deviceId      String which contains the ID of device
     * @param[in]   channelNumber Integer with channel number device is requested to switch to
     * @param[out]  channelName   String with channel name device has switchted to
     * @return      the success of the request
     * @retval      true          request handled properly
     * @retval      false         request was not handled properly because of some error
     * 
     * @section ChangeChannelNumber Example-Code
     * @snippet callbacks.cpp onChangeChannelNumber
     **/
    typedef std::function<bool(const String&, int, String&)> ChangeChannelNumberCallback;

    /**
     * @brief Callback definition for onSkipChannels function
     * 
     * Gets called when device receive a `changeChannel` request by using channel number 
     * @param[in]   deviceId      String which contains the ID of device
     * @param[in]   channelCount  Integer with channels device is requested to skip `-n`..`+n`
     * @param[out]  channelName   String with channel name device has switchted to
     * @return      the success of the request
     * @retval      true          request handled properly
     * @retval      false         request was not handled properly because of some error
     * 
     * @section SkipChannels Example-Code
     * @snippet callbacks.cpp onSkipChannels
     **/
    typedef std::function<bool(const String&, int, String&)> SkipChannelsCallback;

    void onSetVolume(SetVolumeCallback cb);
    void onAdjustVolume(AdjustVolumeCallback cb);
    void onMute(MuteCallback cb);
    void onMediaControl(MediaControlCallback cb);
    void onSelectInput(SelectInputCallback cb);
    void onChangeChannel(ChangeChannelCallback cb);
    void onChangeChannelNumber(ChangeChannelNumberCallback cb);
    void onSkipChannels(SkipChannelsCallback cb);

    // event
    bool sendVolumeEvent(int volume, String cause = "PHYSICAL_INTERACTION");
    bool sendMuteEvent(bool mute, String cause = "PHYSICAL_INTERACTION");
    bool sendMediaControlEvent(String mediaControl, String cause = "PHYSICAL_INTERACTION");
    bool sendSelectInputEvent(String intput, String cause = "PHYSICAL_INTERACTION");
    bool sendChangeChannelEvent(String channelName, String cause = "PHYSICAL_INTERACTION");
    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    SetVolumeCallback volumeCallback;
    AdjustVolumeCallback adjustVolumeCallback; 
    MuteCallback muteCallback;
    MediaControlCallback mediaControlCallback;
    SelectInputCallback selectInputCallback;
    ChangeChannelCallback changeChannelCallback;
    ChangeChannelNumberCallback changeChannelNumberCallback;
    SkipChannelsCallback skipChannelsCallback;
};


SinricProTV::SinricProTV(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  volumeCallback(nullptr),
  adjustVolumeCallback(nullptr),
  muteCallback(nullptr),
  mediaControlCallback(nullptr),
  selectInputCallback(nullptr),
  changeChannelCallback(nullptr),
  changeChannelNumberCallback(nullptr),
  skipChannelsCallback(nullptr) {
}

bool SinricProTV::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

  if (volumeCallback && actionString == "setVolume") {
    int volume = request_value["volume"];
    success = volumeCallback(String(deviceId), volume);
    response_value["volume"] = volume;
    return success;
  }

  if (adjustVolumeCallback && actionString == "adjustVolume") {
    int volume = request_value["volume"];
    success = adjustVolumeCallback(String(deviceId), volume);
    response_value["volume"] = volume;
    return success;
  }

  if (muteCallback && actionString == "setMute") {
    bool mute = request_value["mute"];
    success = muteCallback(String(deviceId), mute);
    response_value["mute"] = mute;
    return success;
  }

  if (mediaControlCallback && actionString == "mediaControl") {
    String mediaControl = request_value["control"];
    success = mediaControlCallback(String(deviceId), mediaControl);
    response_value["control"] = mediaControl;
    return success;
  }

  if (selectInputCallback && actionString == "selectInput") {
    String input = request_value["input"];
    success = selectInputCallback(String(deviceId), input);
    response_value["input"] = input;
    return success;
  }

  if (actionString == "changeChannel") {

    if (changeChannelCallback && request_value["channel"].containsKey("name")) {
      String channelName = request_value["channel"]["name"] | "";
      success = changeChannelCallback(String(deviceId), channelName);
      response_value["channel"]["name"] = channelName;
    }

    if (changeChannelNumberCallback && request_value["channel"].containsKey("number")) {
      int channelNumber = request_value["channel"]["number"];
      String channelName("");
      success = changeChannelNumberCallback(String(deviceId), channelNumber, channelName);
      response_value["channel"]["name"] = channelName;
    }
    return success;
  }

  if (skipChannelsCallback && actionString == "skipChannels") {
    int channelCount = request_value["channelCount"] | 0;
    String channelName;
    success = skipChannelsCallback(String(deviceId), channelCount, channelName);
    response_value["channel"]["name"] = channelName;
    return success;
  }

  return success;
}

/**
 * @brief Set callback function for `setVolume` request
 * 
 * @param cb Function pointer to a `SetVolumeCallback` function
 * @return void
 * @see SetVolumeCallback
 **/
void SinricProTV::onSetVolume(SetVolumeCallback cb) { volumeCallback = cb; }

/**
 * @brief Set callback function for `adjustVolume` request
 * 
 * @param cb Function pointer to a `AdjustVolumeCallback` function
 * @return void
 * @see AdjustVolumeCallback
 **/
void SinricProTV::onAdjustVolume(AdjustVolumeCallback cb) { adjustVolumeCallback = cb; }

/**
 * @brief Set callback function for `setMute` request
 * 
 * @param cb Function pointer to a `MuteCallback` function
 * @return void
 * @see MuteCallback
 **/
void SinricProTV::onMute(MuteCallback cb) { muteCallback = cb; }

/**
 * @brief Set callback function for `mediaControl` request
 * 
 * @param cb Function pointer to a `MediaControlCallback` function
 * @return void
 * @see MediaControlCallback
 **/
void SinricProTV::onMediaControl(MediaControlCallback cb) { mediaControlCallback = cb; }

/**
 * @brief Set callback function for `selectInput` request
 * 
 * @param cb Function pointer to a `SelectInputCallback` function
 * @return void
 * @see SelectInputCallback
 **/
void SinricProTV::onSelectInput(SelectInputCallback cb) { selectInputCallback = cb; }

/**
 * @brief Set callback function for `changeChannel` request
 * 
 * @param cb Function pointer to a `ChangeChannelCallback` function
 * @return void
 * @see ChangeChannelCallback
 **/
void SinricProTV::onChangeChannel(ChangeChannelCallback cb) { changeChannelCallback = cb; }

/**
 * @brief Set callback function for `changeChannel` request
 * 
 * @param cb Function pointer to a `ChangeChannelNumberCallback` function
 * @return void
 * @see ChangeChannelNumberCallback
 **/
void SinricProTV::onChangeChannelNumber(ChangeChannelNumberCallback cb) { changeChannelNumberCallback = cb; }

/**
 * @brief Set callback function for `skipChannels` request
 * 
 * @param cb Function pointer to a `SkipChannelsCallback` function
 * @return void
 * @see SkipChannelsCallback
 **/
void SinricProTV::onSkipChannels(SkipChannelsCallback cb) { skipChannelsCallback = cb; }

/**
 * @brief Send `setVolume` event to SinricPro Server indicating actual volume has changed
 * 
 * @param   volume        Integer reporting the volume that the device have been set to
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProTV::sendVolumeEvent(int volume, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setVolume", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["volume"] = volume;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `setMute` event to SinricPro Server indicating actual mute state
 * 
 * @param mute    `true` = device is muted on \n `false` = device is unmuted
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProTV::sendMuteEvent(bool mute, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setMute", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mute"] = mute;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `mediaControl` event to SinricPro Server indicating devices media control state
 * 
 * @param mediaControl    String with actual media control \n `FastForward`, `Next`, `Pause`, `Play`, `Previous`, `Rewind`, `StartOver`, `Stop`
 * @param cause           (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProTV::sendMediaControlEvent(String mediaControl, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "mediaControl", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["control"] = mediaControl;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `selectInput` event to SinricPro Server to report selected input
 * 
 * @param input           String with actual media control \n `AUX 1`..`AUX 7`, `BLURAY`, `CABLE`, `CD`, `COAX 1`,`COAX 2`, `COMPOSITE 1`, `DVD`, `GAME`, `HD RADIO`, `HDMI 1`.. `HDMI 10`, `HDMI ARC`, `INPUT 1`..`INPUT 10`, `IPOD`, `LINE 1`..`LINE 7`, `MEDIA PLAYER`, `OPTICAL 1`, `OPTICAL 2`, `PHONO`, `PLAYSTATION`, `PLAYSTATION 3`, `PLAYSTATION 4`, `SATELLITE`, `SMARTCAST`, `TUNER`, `TV`, `USB DAC`, `VIDEO 1`..`VIDEO 3`, `XBOX`
 * @param cause           (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProTV::sendSelectInputEvent(String input, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "selectInput", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["input"] = input;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `changeChannel` event to SinricPro Server to report selected channel
 * 
 * @param channelName     String with actual channel
 * @param cause           (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProTV::sendChangeChannelEvent(String channelName, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "changeChannel", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["channel"]["name"] = channelName;
  return sendEvent(eventMessage);
}

#endif

