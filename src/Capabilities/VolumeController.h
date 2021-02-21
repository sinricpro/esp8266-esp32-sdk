#ifndef _VOLUMECONTROLLER_H_
#define _VOLUMECONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief VolumeController
 * @ingroup Capabilities
 **/
template <typename T>
class VolumeController {
  public:
    VolumeController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&VolumeController<T>::handleVolumeController, this, std::placeholders::_1)); }
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
    using SetVolumeCallback = std::function<bool(const String &, int &)>;

    /**
     * @brief Callback definition for onAdjustVolume function
     * 
     * Gets called when device receive a `adjustVolume` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   volumeDelta Integer with relative volume the device should change about (-100..100)
     * @param[out]  volumeDelta Integer with absolute volume device has been set to
     * @param[in]   volumeDefault Bool `false` if the user specified the amount by which to change the volume; otherwise `true`
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustVolumeCallback Example-Code
     * @snippet callbacks.cpp onAdjustVolume
     **/
    using AdjustVolumeCallback = std::function<bool(const String &, int &, bool)>;

    void onSetVolume(SetVolumeCallback cb);
    void onAdjustVolume(AdjustVolumeCallback cb);

    bool sendVolumeEvent(int volume, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleVolumeController(SinricProRequest &request);

  private:
    SetVolumeCallback volumeCallback;
    AdjustVolumeCallback adjustVolumeCallback;
};

/**
 * @brief Set callback function for `setVolume` request
 * 
 * @param cb Function pointer to a `SetVolumeCallback` function
 * @return void
 * @see SetVolumeCallback
 **/
template <typename T>
void VolumeController<T>::onSetVolume(SetVolumeCallback cb) { volumeCallback = cb; }

/**
 * @brief Set callback function for `adjustVolume` request
 * 
 * @param cb Function pointer to a `AdjustVolumeCallback` function
 * @return void
 * @see AdjustVolumeCallback
 **/
template <typename T>
void VolumeController<T>::onAdjustVolume(AdjustVolumeCallback cb) { adjustVolumeCallback = cb; }

/**
 * @brief Send `setVolume` event to SinricPro Server indicating actual volume has changed
 * 
 * @param   volume        Integer reporting the volume that the device have been set to
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool VolumeController<T>::sendVolumeEvent(int volume, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setVolume", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["volume"] = volume;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool VolumeController<T>::handleVolumeController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (volumeCallback && request.action == "setVolume") {
    int volume = request.request_value["volume"];
    success = volumeCallback(device.deviceId, volume);
    request.response_value["volume"] = volume;
    return success;
  }

  if (adjustVolumeCallback && request.action == "adjustVolume") {
    int volume = request.request_value["volume"];
    bool volumeDefault = request.request_value["volumeDefault"] | false;
    success = adjustVolumeCallback(device.deviceId, volume, volumeDefault);
    request.response_value["volume"] = volume;
    return success;
  }
  return success;
}

#endif