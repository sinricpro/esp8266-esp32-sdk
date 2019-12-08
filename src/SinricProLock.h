/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLOCK_H_
#define _SINRICLOCK_H_

#include "SinricProDevice.h"

/**
 * @class SinricProLock
 * @brief Device to control a smart lock
 * 
 * Supporting 
 * * on / off
 * * lock / unlock
 **/
class SinricProLock :  public SinricProDevice {
  public:
	  SinricProLock(const char* deviceId, unsigned long eventWaitTime=100);
    // callback

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
	  typedef std::function<bool(const String&, bool&)> LockStateCallback; // void onLockState(const char* deviceId, bool& lockState);
    
    void onLockState(LockStateCallback cb);
    void onPowerState() = delete;  // SinricProLock has no powerState
    // event
    bool sendPowerStateEvent() = delete; // SinricProLock has no powerState
    bool sendLockStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    LockStateCallback lockStateCallback;
};

SinricProLock::SinricProLock(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  lockStateCallback(nullptr) {}

bool SinricProLock::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  bool success = false;
  String actionString = String(action);

  if (actionString == "setLockState" && lockStateCallback) {
    bool lockState = request_value["state"]=="lock"?true:false;
    success = lockStateCallback(String(deviceId), lockState);
    response_value["state"] = success?lockState?"LOCKED":"UNLOCKED":"JAMMED";
    return success;
  }
  return success;
}

/**
 * @brief Set callback function for `setLockState` request
 * 
 * @param cb Function pointer to a `LockStateCallback` function
 * @return void
 * @see LockStateCallback
 **/
void SinricProLock::onLockState(LockStateCallback cb) { 
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
bool SinricProLock::sendLockStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setLockState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["state"] = "LOCKED" : event_value["state"] = "UNLOCKED";
  return sendEvent(eventMessage);
}
#endif

