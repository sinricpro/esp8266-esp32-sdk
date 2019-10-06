#ifndef _SINRICTV_H_
#define _SINRICTV_H_

#include "SinricProDevice.h"
#include <ArduinoJson.h>

class SinricProTV :  public SinricProDevice {
  public:
	  SinricProTV(const char* deviceId, unsigned long eventWaitTime=100);
    // callback
	  typedef std::function<bool(String, bool&)> PowerStateCallback; // void onPowerState(const char* deviceId, bool& powerState);
    typedef std::function<bool(String, int&)> VolumeCallback;
    typedef std::function<bool(String, bool&)> MuteCallback;
    typedef std::function<bool(String, String&)> MediaControlCallback;
    typedef std::function<bool(String, String&)> SelectInputCallback;
    typedef std::function<bool(String, String&)> ChangeChannelCallback;
    typedef std::function<bool(String, int, String&)> SkipChannelsCallback;

    void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }
    void onSetVolume(VolumeCallback cb) { volumeCallback = cb; }
    void onAdjustVolume(VolumeCallback cb) { adjustVolumeCallback = cb; }
    void onMute(MuteCallback cb) { muteCallback = cb; }
    void onMediaControl(MediaControlCallback cb) { mediaControlCallback = cb; }
    void onSelectInput(SelectInputCallback cb) { selectInputCallback = cb; }
    void onChangeChannel(ChangeChannelCallback cb) { changeChannelCallback = cb; }
    void onSkipChannels(SkipChannelsCallback cb) { skipChannelsCallback = cb; }

    // event
    void sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
    void sendVolumeEvent(int volume, String cause = "PHYSICAL_INTERACTION");
    void sendMuteEvent(bool mute, String cause = "PHYSICAL_INTERACTION");
    void sendMediaControlEvent(String mediaControl, String cause = "PHYSICAL_INTERACTION");
    void sendSelectInputEvent(String intput, String cause = "PHYSICAL_INTERACTION");
    void sendChangeChannelEvent(String channelName, String cause = "PHYSICAL_INTERACTION");
    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    PowerStateCallback powerStateCallback;
    VolumeCallback volumeCallback;
    VolumeCallback adjustVolumeCallback; 
    MuteCallback muteCallback;
    MediaControlCallback mediaControlCallback;
    SelectInputCallback selectInputCallback;
    ChangeChannelCallback changeChannelCallback;
    SkipChannelsCallback skipChannelsCallback;
};


SinricProTV::SinricProTV(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  powerStateCallback(nullptr),
  volumeCallback(nullptr),
  adjustVolumeCallback(nullptr),
  muteCallback(nullptr),
  mediaControlCallback(nullptr),
  selectInputCallback(nullptr),
  changeChannelCallback(nullptr),
  skipChannelsCallback(nullptr) {
}

bool SinricProTV::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
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

  if (changeChannelCallback && actionString == "changeChannel") {
    String channelName = request_value["channel"]["name"] | "";
    success = selectInputCallback(String(deviceId), channelName);
    JsonObject response_channel = response_value["channel"].createNestedObject("name");
    response_channel["name"] = channelName;
    return success;
  }

  if (skipChannelsCallback && actionString == "skipChannels") {
    int channelCount = request_value["channelCount"] | 0;
    String channelName;
    success = skipChannelsCallback(String(deviceId), channelCount, channelName);
    JsonObject response_channel = response_value["channel"].createNestedObject("name");
    response_channel["name"] = channelName;
    return success;
  }

  return success;
}

void SinricProTV::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  sendEvent(eventMessage);
}

void SinricProTV::sendVolumeEvent(int volume, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setVolume", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["volume"] = volume;
  sendEvent(eventMessage);
}

void SinricProTV::sendMuteEvent(bool mute, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setMute", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mute"] = mute;
  sendEvent(eventMessage);
}

void SinricProTV::sendMediaControlEvent(String mediaControl, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "mediaControl", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["control"] = mediaControl;
  sendEvent(eventMessage);
}

void SinricProTV::sendSelectInputEvent(String input, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "selectInput", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["input"] = input;
  sendEvent(eventMessage);
}

void SinricProTV::sendChangeChannelEvent(String channelName, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "changeChannel", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  JsonObject event_channel = event_value["channel"].createNestedObject("name");
  event_channel["name"] = channelName;
  sendEvent(eventMessage);
}


#endif

