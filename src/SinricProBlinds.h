/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICBLINDS_H_
#define _SINRICBLINDS_H_

#include "SinricProDevice.h"

/**
 * @class SinricProBlinds
 * @brief Device to control interior blinds
 * 
 * Supporting 
 * * On / Off
 * * Position (0..100)
 * * open / close 
 **/
class SinricProBlinds :  public SinricProDevice {
  public:
	  SinricProBlinds(const char* deviceId, unsigned long eventWaitTime=100);

    /**
     * @brief Callback definition for onSetPosition function
     * 
     * Gets called when device receive a `setPosition` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   position    Absolute integer value with desired blinds position
     * @param[out]  position    Absolute integer value with actual blinds position
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section SetPositionCallback Example-Code
     * @snippet callbacks.cpp onSetPosition
     **/
    typedef std::function<bool(const String&, int&)> SetPositionCallback;

    /**
     * @brief Callback definition for onAdjustPosition function
     * 
     * Gets called when device receive a `setPowerState` request \n
     * @param[in]   deviceId        String which contains the ID of device
     * @param[in]   positionDelta   Relative integer value the device should change the position of the blinds about (-100..+100)
     * @param[out]  positionDelta   Absolute integer value with the new blinds position (0..100)
     * @return      the success of the request
     * @retval      true            request handled properly
     * @retval      false           request was not handled properly because of some error
     * 
     * @section AdjustPositionCallback Example-Code
     * @snippet callbacks.cpp onAdjustPosition
     **/    
    typedef std::function<bool(const String&, int&)> AdjustPositionCallback;

    void onSetPosition(SetPositionCallback cb);
    void onAdjustPosition(AdjustPositionCallback cb);

    bool sendPositionEvent(int position, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    SetPositionCallback setPositionCallback; 
    AdjustPositionCallback adjustPositionCallback;
};

SinricProBlinds::SinricProBlinds(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  setPositionCallback(nullptr),
  adjustPositionCallback(nullptr) {
}

bool SinricProBlinds::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

  if (actionString == "setRangeValue" && setPositionCallback) {
    int position = request_value["rangeValue"] | 0;
    success = setPositionCallback(String(deviceId), position);
    response_value["rangeValue"] = position;
    return success;
  }

  if (actionString == "adjustRangeValue" && adjustPositionCallback) {
    int positionDelta = request_value["rangeValueDelta"] | 0;
    success = adjustPositionCallback(String(deviceId), positionDelta);
    response_value["rangeValue"] = positionDelta;
    return success;
  }

  return success;
}

/**
 * @brief Set callback function for `setPosition` request
 * 
 * @param cb Function pointer to a `SetPositionCallback` function
 * @return void
 * @see SetPositionCallback
 **/
void SinricProBlinds::onSetPosition(SetPositionCallback cb) { 
  setPositionCallback = cb;
}

/**
 * @brief Set callback function for `adjustPosition` request
 * 
 * @param cb Function pointer to a `AdjustPositionCallback` function
 * @return void
 * @see AdjustPositionCallback
 **/
void SinricProBlinds::onAdjustPosition(AdjustPositionCallback cb) {
  adjustPositionCallback = cb;
}

/**
 * @brief Send `setPosition` event to update actual position on SinricPro Server
 * 
 * @param position `0` = blinds are closed \n `100` = blinds are open
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval 0..100 actual blinds position
 **/
bool SinricProBlinds::sendPositionEvent(int position, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setRangeValue", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = position;
  return sendEvent(eventMessage);
}

#endif

