#ifndef _POWERSTATECONTROLLER_H_
#define _POWERSTATECONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

/**
 * @class PowerStateController
 * @brief support for basic on/off command
 **/
class PowerStateController {
  public:
    PowerStateController(SinricProDeviceInterface *device);

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

    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);
  private:
    SinricProDeviceInterface* device; 
    PowerStateCallback powerStateCallback;
};

PowerStateController::PowerStateController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Set callback function for `powerState` request
 * 
 * @param cb Function pointer to a `PowerStateCallback` function
 * @return void
 * @see PowerStateCallback
 **/
void PowerStateController::onPowerState(PowerStateCallback cb) {
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
bool PowerStateController::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent(device->getDeviceId(), "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "On" : "Off";
  return device->sendEvent(eventMessage);
}

bool PowerStateController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback)  {
    bool powerState = request_value["state"] == "On" ? true : false;
    success = powerStateCallback(device->getDeviceId(), powerState);
    response_value["state"] = powerState ? "On" : "Off";
    return success;
  }
  return success;
}


#endif