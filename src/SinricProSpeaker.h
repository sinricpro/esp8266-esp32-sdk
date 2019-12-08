/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICSPEAKER_H_
#define _SINRICSPEAKER_H_

#include "SinricProDevice.h"

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
class SinricProSpeaker :  public SinricProDevice {
  public:
	  SinricProSpeaker(const char* deviceId, unsigned long eventWaitTime=100);
    // callbacks

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
     * @section SetVolumeCallback Example-Code
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
     * @section AdjustVolumeCallback Example-Code
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
     * @section MuteCallback Example-Code
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
     * @section MediaControlCallback Example-Code
     * @snippet callbacks.cpp onMediaControl
     **/
    typedef std::function<bool(const String&, String&)> MediaControlCallback;

    /**
     * @brief Callback definition for onSetBands function
     * 
     * Gets called when device receive a `setBands` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   bands       String with requested bands to change \n `BASS`, `MIDRANGE`, `TREBBLE`
     * @param[in]   level       Integer value with level bands should set to
     * @param[out]  bands       String with changed bands \n `BASS`, `MIDRANGE`, `TREBBLE`
     * @param[out]  level       Integer value with level bands changed to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SetBandsCallback Example-Code
     * @snippet callbacks.cpp onSetBands
     **/
    typedef std::function<bool(const String&, const String&, int&)> SetBandsCallback;

    /**
     * @brief Callback definition for onAdjustBands function
     * 
     * Gets called when device receive a `adjustBands` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   bands       String with requested bands to change \n `BASS`, `MIDRANGE`, `TREBBLE`
     * @param[in]   levelDelta  Integer with relative level value device should change bands about
     * @param[out]  bands       String with changed bands \n `BASS`, `MIDRANGE`, `TREBBLE`
     * @param[out]  levelDelta  Integer value with level bands changed to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustBandsCallback Example-Code
     * @snippet callbacks.cpp onAdjustBands
     **/
    typedef std::function<bool(const String&, const String&, int&)> AdjustBandsCallback;

    /**
     * @brief Callback definition for onResetBands function
     * 
     * Gets called when device receive a `onResetBands` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   bands       String with requested bands to reset \n `BASS`, `MIDRANGE`, `TREBBLE`
     * @param[out]  bands       String with changed bands \n `BASS`, `MIDRANGE`, `TREBBLE`
     * @param[out]  level       Integer value with level bands changed to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section ResetBandsCallback Example-Code
     * @snippet callbacks.cpp onResetBands
     **/    
    typedef std::function<bool(const String&, const String&, int&)> ResetBandsCallback;

    /**
     * @brief Callback definition for onSetMode function
     * 
     * Gets called when device receive a `setBands` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   mode        String device mode should set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
     * @param[out]  mode        String devices mode is set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section ModeCallback Example-Code
     * @snippet callbacks.cpp onSetMode
     **/    typedef std::function<bool(const String&, String&)> ModeCallback;

    void onSetVolume(SetVolumeCallback cb);
    void onAdjustVolume(AdjustVolumeCallback cb);
    void onMute(MuteCallback cb);
    void onMediaControl(MediaControlCallback cb);
    void onSetBands(SetBandsCallback cb);
    void onAdjustBands(AdjustBandsCallback cb);
    void onResetBands(ResetBandsCallback cb);
    void onSetMode(ModeCallback cb);

    // event
    bool sendVolumeEvent(int volume, String cause = "PHYSICAL_INTERACTION");
    bool sendMuteEvent(bool mute, String cause = "PHYSICAL_INTERACTION");
    bool sendMediaControlEvent(String mediaControl, String cause = "PHYSICAL_INTERACTION");
    bool sendBandsEvent(String bands, int level, String cause = "PHYSICAL_INTERACTION");
    bool sendModeEvent(String mode, String cause = "PHYSICAL_INTERACTION");
    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    SetVolumeCallback volumeCallback;
    AdjustVolumeCallback adjustVolumeCallback; 
    MuteCallback muteCallback;
    MediaControlCallback mediaControlCallback;
    SetBandsCallback setBandsCallback;
    AdjustBandsCallback adjustBandsCallback;
    ResetBandsCallback resetBandsCallback;
    ModeCallback setModeCallback;
};


SinricProSpeaker::SinricProSpeaker(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  volumeCallback(nullptr),
  adjustVolumeCallback(nullptr),
  muteCallback(nullptr),
  mediaControlCallback(nullptr),
  setBandsCallback(nullptr),
  adjustBandsCallback(nullptr),
  resetBandsCallback(nullptr),
  setModeCallback(nullptr) {
}

bool SinricProSpeaker::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
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

  if (setModeCallback && actionString == "setMode") {
    String mode = request_value["mode"] | "";
    success = setModeCallback(String(deviceId), mode);
    response_value["mode"] = mode;
    return success;
  }

  if (mediaControlCallback && actionString == "mediaControl") {
    String mediaControl = request_value["control"];
    success = mediaControlCallback(String(deviceId), mediaControl);
    response_value["control"] = mediaControl;
    return success;
  }

  if (setBandsCallback && actionString == "setBands") {
    JsonArray bands_array = request_value["bands"];
    JsonArray response_value_bands = response_value.createNestedArray("bands");

    for (size_t i=0; i<bands_array.size(); i++) {
      int level = bands_array[i]["level"] | 0;
      String bandsName = bands_array[i]["name"] | "";
      success = setBandsCallback(deviceId, bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = level;
    }
    return success;
  }

  if (setBandsCallback && actionString == "adjustBands") {
    JsonArray bands_array = request_value["bands"];
    JsonArray response_value_bands = response_value.createNestedArray("bands");

    for (size_t i=0; i<bands_array.size(); i++) {
      int levelDelta = bands_array[i]["levelDelta"] | 1;
      String direction = bands_array[i]["levelDirection"];
      if (direction=="DOWN") levelDelta *= -1;
      String bandsName = bands_array[i]["name"] | "";
      success = adjustBandsCallback(deviceId, bandsName, levelDelta);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = levelDelta;
    }
    return success;
  }

  if (resetBandsCallback && actionString == "resetBands") {
    JsonArray bands_array = request_value["bands"];
    JsonArray response_value_bands = response_value.createNestedArray("bands");

    for (size_t i=0; i<bands_array.size(); i++) {
      int level = 0;
      String bandsName = bands_array[i]["name"] | "";
      success = adjustBandsCallback(deviceId, bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = level;
    }
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
void SinricProSpeaker::onSetVolume(SetVolumeCallback cb) { volumeCallback = cb; }

/**
 * @brief Set callback function for `adjustVolume` request
 * 
 * @param cb Function pointer to a `AdjustVolumeCallback` function
 * @return void
 * @see AdjustVolumeCallback
 **/
void SinricProSpeaker::onAdjustVolume(AdjustVolumeCallback cb) { adjustVolumeCallback = cb; }

/**
 * @brief Set callback function for `setMute` request
 * 
 * @param cb Function pointer to a `MuteCallback` function
 * @return void
 * @see MuteCallback
 **/
void SinricProSpeaker::onMute(MuteCallback cb) { muteCallback = cb; }

/**
 * @brief Set callback function for `mediaControl` request
 * 
 * @param cb Function pointer to a `MediaControlCallback` function
 * @return void
 * @see MediaControlCallback
 **/
void SinricProSpeaker::onMediaControl(MediaControlCallback cb) { mediaControlCallback = cb; }

/**
 * @brief Set callback function for `setBands` request
 * 
 * @param cb Function pointer to a `SetBandsCallback` function
 * @return void
 * @see SetBandsCallback
 **/
void SinricProSpeaker::onSetBands(SetBandsCallback cb) { setBandsCallback = cb; }

/**
 * @brief Set callback function for `adjustBands` request
 * 
 * @param cb Function pointer to a `AdjustBandsCallback` function
 * @return void
 * @see AdjustBandsCallback
 **/
void SinricProSpeaker::onAdjustBands(AdjustBandsCallback cb) { adjustBandsCallback = cb; }

/**
 * @brief Set callback function for `resetBands` request
 * 
 * @param cb Function pointer to a `ResetBandsCallback` function
 * @return void
 * @see ResetBandsCallback
 **/
void SinricProSpeaker::onResetBands(ResetBandsCallback cb) { resetBandsCallback = cb; }

/**
 * @brief Set callback function for `setMode` request
 * 
 * @param cb Function pointer to a `ModeCallback` function
 * @return void
 * @see ModeCallback
 **/
void SinricProSpeaker::onSetMode(ModeCallback cb) { setModeCallback = cb; }

/**
 * @brief Send `setVolume` event to SinricPro Server indicating actual volume has changed
 * 
 * @param   volume        Integer reporting the volume that the device have been set to
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProSpeaker::sendVolumeEvent(int volume, String cause) {
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
bool SinricProSpeaker::sendMuteEvent(bool mute, String cause) {
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
bool SinricProSpeaker::sendMediaControlEvent(String mediaControl, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "mediaControl", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["control"] = mediaControl;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `setMode` event to SinricPro Server indicating the mode has changed
 * 
 * @param mode    String with actual mode device is set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProSpeaker::sendModeEvent(String mode, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mode"] = mode;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `setBands` event to SinricPro Server indicating bands level has changed
 * 
 * @param bands   String which bands has changed \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param level   Integer with changed bands level
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProSpeaker::sendBandsEvent(String bands, int level, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setBands", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  JsonArray event_value_bands = event_value.createNestedArray("bands");
  JsonObject event_bands = event_value_bands.createNestedObject();
  event_bands["name"] = bands;
  event_bands["value"] = level;
  return sendEvent(eventMessage);
}


#endif

