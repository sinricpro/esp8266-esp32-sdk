#ifndef _POWERSTATECONTROLLER_H_
#define _POWERSTATECONTROLLER_H_

/**
 * @brief PowerStateController
 * @ingroup Controller
 **/
template <typename T>
class PowerStateController {
  public:
    /**
     * @brief Callback definition for onPowerState function
     * 
     * Gets called when device receive a `setPowerState` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   state       `true` = device is requested to turn on \n `false` = device is requested to turn off
     * @param[out]  state       `true` = device has been turned on \n `false` = device has been turned off
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section PowerStateCallback Example-Code
     * @snippet callbacks.cpp onPowerState
     **/
    using PowerStateCallback = std::function<bool(const String &, bool &)>;

    void onPowerState(PowerStateCallback cb);
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handlePowerStateController(const String &action, JsonObject &request_value, JsonObject &response_value);

  private:
    PowerStateCallback powerStateCallback;
};

/**
 * @brief Set callback function for `powerState` request
 * 
 * @param cb Function pointer to a `PowerStateCallback` function
 * @return void
 * @see PowerStateCallback
 **/
template <typename T>
void PowerStateController<T>::onPowerState(PowerStateCallback cb) {
  powerStateCallback = cb;
}

/**
 * @brief Send `setPowerState` event to SinricPro Server indicating actual power state
 * 
 * @param state   `true` = device turned on \n `false` = device turned off
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool PowerStateController<T>::sendPowerStateEvent(bool state, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "On" : "Off";
  return device.sendEvent(eventMessage);
}

template <typename T>
bool PowerStateController<T>::handlePowerStateController(const String &action, JsonObject &request_value, JsonObject &response_value) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (action == "setPowerState" && powerStateCallback)  {
    bool powerState = request_value["state"] == "On" ? true : false;
//    success = powerStateCallback(device.deviceId, powerState);
    success = powerStateCallback(device.deviceId, powerState);
    response_value["state"] = powerState ? "On" : "Off";
    return success;
  }
  return success;
}

#endif