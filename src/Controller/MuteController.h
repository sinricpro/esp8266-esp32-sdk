#ifndef _MUTECONTROLLER_H_
#define _MUTECONTROLLER_H_

/**
 * @brief MuteController
 * @ingroup Controller
 **/
template <typename T>
class MuteController {
  public:

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
    using MuteCallback = std::function<bool(const String &, bool &)>;

    void onMute(MuteCallback cb);
    bool sendMuteEvent(bool mute, String cause = "PHYSICAL_INTERACTION");
  protected:
    bool handleMuteController(const String &action, JsonObject &request_value, JsonObject &response_value);

  private:
    MuteCallback muteCallback;
};

/**
 * @brief Set callback function for `setMute` request
 * 
 * @param cb Function pointer to a `MuteCallback` function
 * @return void
 * @see MuteCallback
 **/
template <typename T>
void MuteController<T>::onMute(MuteCallback cb) { muteCallback = cb; }

/**
 * @brief Send `setMute` event to SinricPro Server indicating actual mute state
 * 
 * @param mute    `true` = device is muted on \n `false` = device is unmuted
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool MuteController<T>::sendMuteEvent(bool mute, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setMute", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mute"] = mute;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool MuteController<T>::handleMuteController(const String &action, JsonObject &request_value, JsonObject &response_value) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (muteCallback && action == "setMute") {
    bool mute = request_value["mute"];
    success = muteCallback(device.deviceId, mute);
    response_value["mute"] = mute;
    return success;
  }
  return success;
}

#endif