#ifndef _MEDIACONTROLLER_H_
#define _MEDIACONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class MediaController {
  public:
    MediaController(SinricProDeviceInterface *device);

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

    void onMediaControl(MediaControlCallback cb);
    bool sendMediaControlEvent(String mediaControl, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
    SinricProDeviceInterface* device;
    MediaControlCallback mediaControlCallback;
};

MediaController::MediaController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Set callback function for `mediaControl` request
 * 
 * @param cb Function pointer to a `MediaControlCallback` function
 * @return void
 * @see MediaControlCallback
 **/
void MediaController::onMediaControl(MediaControlCallback cb) { 
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
bool MediaController::sendMediaControlEvent(String mediaControl, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("mediaControl", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["control"] = mediaControl;
  return device->sendEvent(eventMessage);
}

bool MediaController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (mediaControlCallback && actionString == "mediaControl") {
    String mediaControl = request_value["control"];
    success = mediaControlCallback(device->getDeviceId(), mediaControl);
    response_value["control"] = mediaControl;
    return success;
  }

  return success;
}

#endif