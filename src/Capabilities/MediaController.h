#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(MEDIA, mediaControl);   // "mediaControl"
FSTR(MEDIA, control);        // "control"

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
using MediaControlCallback = std::function<bool(const String &, String &)>;


/**
 * @brief MediaController
 * @ingroup Capabilities
 **/
template <typename T>
class MediaController {
  public:
    MediaController();

    void onMediaControl(MediaControlCallback cb);
    bool sendMediaControlEvent(String mediaControl, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
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

/**
 * @brief Set callback function for `mediaControl` request
 * 
 * @param cb Function pointer to a `MediaControlCallback` function
 * @return void
 * @see MediaControlCallback
 **/
template <typename T>
void MediaController<T>::onMediaControl(MediaControlCallback cb) {
  mediaControlCallback = cb;
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
template <typename T>
bool MediaController<T>::sendMediaControlEvent(String mediaControl, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_MEDIA_mediaControl, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_MEDIA_control] = mediaControl;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool MediaController<T>::handleMediaController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (mediaControlCallback && request.action == FSTR_MEDIA_mediaControl) {
    String mediaControl = request.request_value[FSTR_MEDIA_control];
    success = mediaControlCallback(device->deviceId, mediaControl);
    request.response_value[FSTR_MEDIA_control] = mediaControl;
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using MediaController = SINRICPRO_NAMESPACE::MediaController<T>;