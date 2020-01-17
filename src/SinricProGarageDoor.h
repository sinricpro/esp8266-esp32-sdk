/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICGARAGEDOOR_H_
#define _SINRICGARAGEDOOR_H_

#include "SinricProDevice.h"

/**
 * @class SinricProGarageDoor
 * @brief Device to control a garage door
 * 
 * Supporting 
 * * open / close 
 **/
class SinricProGarageDoor :  public SinricProDevice {
  public:
	  SinricProGarageDoor(const char* deviceId, unsigned long eventWaitTime=100);
        
    /**
     * @brief Callback definition for onDoorState function
     * 
     * Gets called when device receive a `open` or `close` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   doorState   `false` = open, device is requested to open the garage door \n`true` = close, device is requested to close the garage door
     * @param[out]  doorState   bool with actual state `false` = open, `true` = closed
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section DoorStateCallback Example-Code
     * @snippet callbacks.cpp onDoorState
     **/
    typedef std::function<bool(const String&, bool&)> DoorStateCallback;
    
    void onDoorState(DoorStateCallback cb);
    void onPowerState() = delete;  // SinricProGarageDoor has no powerState

    // event
    bool sendDoorStateEvent(bool mode, String cause = "PHYSICAL_INTERACTION");
    bool sendPowerStateEvent() = delete; // SinricProGarageDoor has no powerState

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    DoorStateCallback doorStateCallback;
};

SinricProGarageDoor::SinricProGarageDoor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  doorStateCallback(nullptr) {
}

bool SinricProGarageDoor::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

  if (actionString == "setMode" && doorStateCallback) {
    String modeStr = request_value["mode"] | "";
    bool mode;
    if (modeStr == "Open") mode = false;
    if (modeStr == "Close") mode = true;
    success = doorStateCallback(String(deviceId), mode);
    if (mode == false) modeStr = "Open";
    if (mode == true) modeStr = "Close";
    response_value["mode"] = modeStr;
    return success;
  }
  return success;
}

/**
 * @brief Set callback function for `onDoorState` request
 * 
 * @param cb Function pointer to a `onDoorState` function
 * @return void
 * @see DoorStateCallback
 **/
void SinricProGarageDoor::onDoorState(DoorStateCallback cb) {
  doorStateCallback = cb;
}

/**
 * @brief Send `DoorState` event to update actual door state on SinricPro Server
 * 
 * @param state   bool `true` = close \n `false` = open
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProGarageDoor::sendDoorStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["mode"] = "Close" : event_value["mode"] = "Open";
  return sendEvent(eventMessage);
}

#endif

