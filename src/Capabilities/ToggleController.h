#ifndef _TOGGLECONTROLLER_H_
#define _TOGGLECONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief ToggleController
 * @ingroup Capabilities
 **/
template <typename T>
class ToggleController {
public:
  ToggleController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&ToggleController<T>::handleToggleController, this, std::placeholders::_1)); }
  /**
     * @brief Callback definition for onToggleState function
     * 
     * Gets called when device receive a `setPowerState` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   instance    String which instance is requested
     * @param[in]   state       `true` = device is requested to turn on \n `false` = device is requested to turn off
     * @param[out]  state       `true` = device has been turned on \n `false` = device has been turned off
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section ToggleStateCallback Example-Code
     * @snippet callbacks.cpp onToggleState
     **/
  using GenericToggleStateCallback = std::function<bool(const String &, const String&, bool &)>;

  void onToggleState(const String& instance, GenericToggleStateCallback cb);
  bool sendToggleStateEvent(const String &instance, bool state, String cause = "PHYSICAL_INTERACTION");

protected:
  bool handleToggleController(SinricProRequest &request);

private:
  std::map<String, GenericToggleStateCallback> genericToggleStateCallback;
};


/**
 * @brief Set callback function for `toggleState` request
 * 
 * @param instance String instance name (custom device)
 * @param cb Function pointer to a `ToggleStateCallback` function
 * @return void
 * @see ToggleStateCallback
 **/
template <typename T>
void ToggleController<T>::onToggleState(const String &instance, GenericToggleStateCallback cb) {
  genericToggleStateCallback[instance] = cb;
}

/**
 * @brief Send `setToggleState` event to SinricPro Server indicating actual toggle state
 * 
 * @param instance String instance name (custom device)
 * @param state   `true` = state turned on \n `false` = tate turned off
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ToggleController<T>::sendToggleStateEvent(const String &instance, bool state, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setToggleState", cause.c_str());
  eventMessage["payload"]["instanceId"] = instance;
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "On" : "Off";
  return device.sendEvent(eventMessage);
}

template <typename T>
bool ToggleController<T>::handleToggleController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (request.action == "setToggleState")  {
    bool powerState = request.request_value["state"] == "On" ? true : false;
    if (genericToggleStateCallback.find(request.instance) != genericToggleStateCallback.end())
      success = genericToggleStateCallback[request.instance](device.deviceId, request.instance, powerState);
    request.response_value["state"] = powerState ? "On" : "Off";
    return success;
  }
  return success;
}

#endif