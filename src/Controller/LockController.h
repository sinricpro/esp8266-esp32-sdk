#ifndef _LOCKCONTROLLER_H_
#define _LOCKCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

/**
 * @class LockController
 * @brief supports onLockState, sendLockStateEvent
 **/
class LockController {
  public:
    LockController(SinricProDeviceInterface* device);

    /**
     * @brief Callback definition for onLockState function
     * 
     * Gets called when device receive a `setLockState` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   state       `true` = device is requested to lock \n `false` = device is requested to unlock
     * @param[out]  state       `true` = device has been locked \n `false` = device has been unlocked
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section LockStateCallback Example-Code
     * @code
     * bool onLockState(const String &deviceId, bool &lockState) {
     *   Serial.printf("Device is %s\r\n", lockState?"locked":"unlocked");
     *   return true;
     * }
     * @endcode
     **/
    using LockStateCallback = std::function<bool(const String &, bool &)>; // void onLockState(const DeviceId &deviceId, bool& lockState);

    void onLockState(LockStateCallback cb);
    bool sendLockStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
    SinricProDeviceInterface* device;
    LockStateCallback lockStateCallback;
};

LockController::LockController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Set callback function for `setLockState` request
 * 
 * @param cb Function pointer to a `LockStateCallback` function
 * @return void
 * @see LockStateCallback
 **/
void LockController::onLockState(LockStateCallback cb) {
  lockStateCallback = cb;
}

/**
 * @brief Send `lockState` event to SinricPro Server indicating actual lock state
 * 
 * @param state   `true` = device is locked \n `false` = device is unlocked
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool LockController::sendLockStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent(device->getDeviceId(), "setLockState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["state"] = "LOCKED" : event_value["state"] = "UNLOCKED";
  return device->sendEvent(eventMessage);
}

bool LockController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  String actionString = String(action);
  bool success = false;

  if (actionString == "setLockState" && lockStateCallback)  {
    bool lockState = request_value["state"] == "lock" ? true : false;
    success = lockStateCallback(device->getDeviceId(), lockState);
    response_value["state"] = success ? lockState ? "LOCKED" : "UNLOCKED" : "JAMMED";
    return success;
  }
  return success;
}

#endif