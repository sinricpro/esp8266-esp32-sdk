#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using ChangeChannelCallback = std::function<bool(const String &, String &)>;
using ChangeChannelNumberCallback = std::function<bool(const String &, int, String &)>;
using SkipChannelsCallback = std::function<bool(const String &, int, String &)>;

template <typename T>
class ChannelController {
  public:
    ChannelController();

    void onChangeChannel(ChangeChannelCallback cb);
    void onChangeChannelNumber(ChangeChannelNumberCallback cb);
    void onSkipChannels(SkipChannelsCallback cb);

    bool sendChangeChannelEvent(String channelName, String cause = "PHYSICAL_INTERACTION");
  protected:
    virtual bool onChangeChannel(String &channelName);
    virtual bool onChangeChannelNumber(int channelNumber, String &channelName);
    virtual bool onSkipChannels(int channelCount, String &channelName);

    bool handleChannelController(SinricProRequest &request);

  private:
    EventLimiter event_limiter; 
    ChangeChannelCallback changeChannelCallback;
    ChangeChannelNumberCallback changeChannelNumberCallback;
    SkipChannelsCallback skipChannelsCallback;
};

template <typename T>
ChannelController<T>::ChannelController()
: event_limiter(EVENT_LIMIT_STATE) { 
  static_cast<T*>(this)->registerRequestHandler(std::bind(&ChannelController<T>::handleChannelController, this, std::placeholders::_1)); 
}

template <typename T>
void ChannelController<T>::onChangeChannel(ChangeChannelCallback cb) {
  changeChannelCallback = cb;
}

template <typename T>
void ChannelController<T>::onChangeChannelNumber(ChangeChannelNumberCallback cb) {
  changeChannelNumberCallback = cb;
}

template <typename T>
void ChannelController<T>::onSkipChannels(SkipChannelsCallback cb) {
  skipChannelsCallback = cb;
}

template <typename T>
bool ChannelController<T>::onChangeChannel(String &channelName) {
  T* device = static_cast<T*>(this);
  if (changeChannelCallback) return changeChannelCallback(device->deviceId, channelName);
  return false;
}

template <typename T>
bool ChannelController<T>::onChangeChannelNumber(int channelNumber, String &channelName) {
  T* device = static_cast<T*>(this);
  if (changeChannelNumberCallback) return changeChannelNumberCallback(device->deviceId, channelNumber, channelName);
  return false;
}

template <typename T>
bool ChannelController<T>::onSkipChannels(int channelCount, String &channelName) {
  T* device = static_cast<T*>(this);
  if (skipChannelsCallback) return skipChannelsCallback(device->deviceId, channelCount, channelName);
  return false;
}

template <typename T>
bool ChannelController<T>::sendChangeChannelEvent(String channelName, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("changeChannel", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["channel"]["name"] = channelName;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ChannelController<T>::handleChannelController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "changeChannel") {

    if (request.request_value["channel"].containsKey("name")) {
      String channelName = request.request_value["channel"]["name"] | "";
      success = onChangeChannel(channelName);
      request.response_value["channel"]["name"] = channelName;
    }

    if (request.request_value["channel"].containsKey("number")) {
      int channelNumber = request.request_value["channel"]["number"];
      String channelName("");
      success = onChangeChannelNumber(channelNumber, channelName);
      request.response_value["channel"]["name"] = channelName;
    }
    return success;
  }

  if (request.action == "skipChannels") {
    int channelCount = request.request_value["channelCount"] | 0;
    String channelName;
    success = onSkipChannels(channelCount, channelName);
    request.response_value["channel"]["name"] = channelName;
    return success;
  }

  return success;
}

// #endif

} // SINRICPRO_NAMESPACE