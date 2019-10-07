#ifndef _SINRICSPEAKER_H_
#define _SINRICSPEAKER_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProSpeaker :  public SinricProDevice {
  public:
	  SinricProSpeaker(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    typedef std::function<bool(String, int&)> VolumeCallback;
    typedef std::function<bool(String, bool&)> MuteCallback;
    typedef std::function<bool(String, String&)> MediaControlCallback;
    typedef std::function<bool(String, String, int&)> BandsCallback;
    typedef std::function<bool(String, String&)> ModeCallback;

    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }
    void onSetVolume(VolumeCallback cb) { volumeCallback = cb; }
    void onAdjustVolume(VolumeCallback cb) { adjustVolumeCallback = cb; }
    void onMute(MuteCallback cb) { muteCallback = cb; }
    void onMediaControl(MediaControlCallback cb) { mediaControlCallback = cb; }
    void onSetBands(BandsCallback cb) { setBandsCallback = cb; }
    void onAdjustBands(BandsCallback cb) { adjustBandsCallback = cb; }
    void onResetBands(BandsCallback cb) { resetBandsCallback = cb; }
    void onSetMode(ModeCallback cb) { setModeCallback = cb; }

    // event
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    bool sendVolumeEvent(int volume, String cause = "PHYSICAL_INTERACTION");
    bool sendMuteEvent(bool mute, String cause = "PHYSICAL_INTERACTION");
    bool sendMediaControlEvent(String mediaControl, String cause = "PHYSICAL_INTERACTION");
    bool sendBandsEvent(String bands, int level, String cause = "PHYSICAL_INTERACTION");
    bool sendModeEvent(String mode, String cause = "PHYSICAL_INTERACTION");
    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
    VolumeCallback volumeCallback;
    VolumeCallback adjustVolumeCallback; 
    MuteCallback muteCallback;
    MediaControlCallback mediaControlCallback;
    BandsCallback setBandsCallback;
    BandsCallback adjustBandsCallback;
    BandsCallback resetBandsCallback;
    ModeCallback setModeCallback;
};


SinricProSpeaker::SinricProSpeaker(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr),
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
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(String(deviceId), powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }

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

bool SinricProSpeaker::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  return sendEvent(eventMessage);
}

bool SinricProSpeaker::sendVolumeEvent(int volume, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setVolume", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["volume"] = volume;
  return sendEvent(eventMessage);
}

bool SinricProSpeaker::sendMuteEvent(bool mute, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setMute", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mute"] = mute;
  return sendEvent(eventMessage);
}

bool SinricProSpeaker::sendMediaControlEvent(String mediaControl, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "mediaControl", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["control"] = mediaControl;
  return sendEvent(eventMessage);
}

bool SinricProSpeaker::sendModeEvent(String mode, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mode"] = mode;
  return sendEvent(eventMessage);
}

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

