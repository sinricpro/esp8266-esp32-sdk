#ifndef _VOLUMECONTROLLER_H_
#define _VOLUMECONTROLLER_H_

/**
 * @brief VolumeController
 * @ingroup Controller
 **/
template <typename T>
class VolumeController {
  public:

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
    bool handleVolumeController(const String &action, JsonObject &request_value, JsonObject &response_value);

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
bool VolumeController<T>::handleVolumeController(const String &action, JsonObject &request_value, JsonObject &response_value) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (volumeCallback && action == "setVolume") {
    int volume = request_value["volume"];
    success = volumeCallback(device.getDeviceId(), volume);
    response_value["volume"] = volume;
    return success;
  }

  if (adjustVolumeCallback && action == "adjustVolume") {
    int volume = request_value["volume"];
    bool volumeDefault = request_value["volumeDefault"] | false;
    success = adjustVolumeCallback(device.getDeviceId(), volume, volumeDefault);
    response_value["volume"] = volume;
    return success;
  }
  return success;
}

#endif