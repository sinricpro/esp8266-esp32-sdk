/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICFANUS_H_
#define _SINRICFANUS_H_

#include "SinricProDevice.h"

/**
 * @class SinricProFanUS
 * @brief Device to control a fan with on / off commands and its speed by a range value
 */
class SinricProFanUS :  public SinricProDevice {
  public:
	  SinricProFanUS(const char* deviceId, unsigned long eventWaitTime=100);
    // callback

    /**
     * @brief Callback definition for onRangeValue function
     * 
     * Gets called when device receive a `setRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer 0..3 for range value device has to be set
     * @param[out]  rangeValue  Integer 0..3 returning the current range value
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SetRangeValueCallback Example-Code
     * @snippet callbacks.cpp onRangeValue
     **/
    typedef std::function<bool(const String&, int&)> SetRangeValueCallback;

    /**
     * @brief Callback definition for onAdjustRangeValue function
     * 
     * Gets called when device receive a `adjustRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer -3..3 delta value for range value have to change
     * @param[out]  rangeValue  Integer 3..3 returning the absolute range value 
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustRangeValueCallback Example-Code
     * @snippet callbacks.cpp onAdjustRangeValue
     **/
    typedef std::function<bool(const String&, int&)> AdjustRangeValueCallback;

    void onRangeValue(SetRangeValueCallback cb);

    void onAdjustRangeValue(AdjustRangeValueCallback cb);

    // event
    bool sendRangeValueEvent(int rangeValue, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    SetRangeValueCallback setRangeValueCallback; 
    AdjustRangeValueCallback adjustRangeValueCallback;
};

SinricProFanUS::SinricProFanUS(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  setRangeValueCallback(nullptr) {}

bool SinricProFanUS::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

  if (actionString == "setRangeValue" && setRangeValueCallback) {
    int rangeValue = request_value["rangeValue"] | 0;
    success = setRangeValueCallback(String(deviceId), rangeValue);
    if (rangeValue < 1) rangeValue = 1;
    if (rangeValue > 3) rangeValue = 3;
    response_value["rangeValue"] = rangeValue;
    return success;
  }

  if (actionString == "adjustRangeValue" && adjustRangeValueCallback) {
    int rangeValueDelta = request_value["rangeValueDelta"] | 0;
    success = adjustRangeValueCallback(String(deviceId), rangeValueDelta);
    if (rangeValueDelta < 1) rangeValueDelta = 1;
    if (rangeValueDelta > 3) rangeValueDelta = 3;
    response_value["rangeValue"] = rangeValueDelta;
    return success;
  }

  return success;
}

/**
 * @brief Set callback function for `setRangeValue` request
 * 
 * @param cb Function pointer to a `SetRangeValueCallback` function
 * @see SetRangeValueCallback
 */
void SinricProFanUS::onRangeValue(SetRangeValueCallback cb) { 
  setRangeValueCallback = cb;
}

/**
 * @brief Set callback function for `adjustRangeValue` request
 * 
 * @param cb Function pointer to a `AdjustRangeValueCallback` function
 * @see AdjustRangeValueCallback
 */
void SinricProFanUS::onAdjustRangeValue(AdjustRangeValueCallback cb) { 
  adjustRangeValueCallback = cb; 
}

/**
 * @brief Send `rangeValue` event to report curent rangeValue to SinricPro server
 * 
 * @param   rangeValue  Value between 0..3
 * @param   cause       (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true        event has been sent successfully
 * @retval  false       event has not been sent, maybe you sent to much events in a short distance of time
 */
bool SinricProFanUS::sendRangeValueEvent(int rangeValue, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setRangeValue", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return sendEvent(eventMessage);
}

#endif

