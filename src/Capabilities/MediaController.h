#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using MediaControlCallback = std::function<bool(const String &, String &)>;

template <typename T>
class MediaController {
  public:
    MediaController();

    void onMediaControl(MediaControlCallback cb);
    bool sendMediaControlEvent(String mediaControl, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onMediaControl(String &mediaControl);
    bool handleMediaController(SinricProRequest &request);

  private:
    EventLimiter event_limiter; 
    MediaControlCallback mediaControlCallback;
};

template <typename T>
MediaController<T>::MediaController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&MediaController<T>::handleMediaController, this, std::placeholders::_1)); 
}

template <typename T>
void MediaController<T>::onMediaControl(MediaControlCallback cb) {
  mediaControlCallback = cb;
}

template <typename T>
bool MediaController<T>::onMediaControl(String &mediaControl) {
  T* device = static_cast<T*>(this);
  if (mediaControlCallback) return mediaControlCallback(device->deviceId, mediaControl);
  return false;
}

template <typename T>
bool MediaController<T>::sendMediaControlEvent(String mediaControl, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("mediaControl", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["control"] = mediaControl;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool MediaController<T>::handleMediaController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "mediaControl") {
    String mediaControl = request.request_value["control"];
    success = onMediaControl(mediaControl);
    request.response_value["control"] = mediaControl;
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE