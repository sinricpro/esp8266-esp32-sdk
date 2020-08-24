/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDEVICE_H_
#define _SINRICDEVICE_H_

#include "SinricProDeviceInterface.h"
#include "LeakyBucket.h"
#include "SinricProId.h"

#include <map>

/**
 * @class SinricProDevice
 * @brief Base class for all device types
 * 
 * Supporting base functions which needed by all device types to work with SinricProClass \n
 * Implements basic on/off functions like onPowerState and sendPowerStateEvent
 **/
class SinricProDevice : public SinricProDeviceInterface {
  public:
    SinricProDevice(const DeviceId &deviceId);
    virtual ~SinricProDevice();
    virtual DeviceId getDeviceId();
    virtual String getProductType();
    virtual void begin(SinricProInterface* eventSender);

    // callback definitions
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
    typedef std::function<bool(const String&, bool&)> PowerStateCallback;
    

    // standard request handler
    virtual bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value);

    // standard Callbacks
    virtual void onPowerState(PowerStateCallback cb);

    // standard events
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool sendEvent(JsonDocument& event);
    virtual DynamicJsonDocument prepareEvent(const DeviceId &deviceId, const char* action, const char* cause);
    unsigned long getTimestamp();
    DeviceId deviceId;
    PowerStateCallback powerStateCallback;
    template <typename T>
    T limitValue(T value, T minValue, T maxValue);
  private:
    SinricProInterface* eventSender;
    std::map<String, LeakyBucket_t> eventFilter;
};

SinricProDevice::SinricProDevice(const DeviceId &deviceId) : 
  deviceId(deviceId),
  powerStateCallback(nullptr),
  eventSender(nullptr) {
}

SinricProDevice::~SinricProDevice() {
}

void SinricProDevice::begin(SinricProInterface* eventSender) {
  this->eventSender = eventSender;
}

DeviceId SinricProDevice::getDeviceId() {
  return deviceId;
}

bool SinricProDevice::handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (deviceId != this->deviceId) return false;
  DEBUG_SINRIC("SinricProDevice::handleRequest()\r\n");
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(deviceId, powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }
  return success;
}

DynamicJsonDocument SinricProDevice::prepareEvent(const DeviceId &deviceId, const char* action, const char* cause) {
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
  DEBUG_SINRIC("[SinricProDevice:prepareEvent()]: Device \"%s\" isn't configured correctly! The \'%s\' event will be ignored.\r\n", deviceId.toString().c_str(), action);
  return DynamicJsonDocument(1024);
}


bool SinricProDevice::sendEvent(JsonDocument& event) {
  if (!eventSender) return false;
  if (!eventSender->isConnected()) {
    DEBUG_SINRIC("[SinricProDevice::sendEvent]: The event could not be sent. No connection to the SinricPro server.\r\n");
    return false;
  }
  String eventName = event["payload"]["action"] | ""; // get event name

  LeakyBucket_t bucket; // leaky bucket algorithm is used to prevent flooding the server

  // get leaky bucket for event from eventFilter
  if (eventFilter.find(eventName) == eventFilter.end()) {  // if there is no bucket ...
    eventFilter[eventName] = bucket;                       // ...add a new bucket
  } else {  
    bucket = eventFilter[eventName];                       // else get bucket
  }

  if (bucket.addDrop()) {                                  // if we can add a new drop
    eventSender->sendMessage(event);                       // send event
    eventFilter[eventName] = bucket;                       // update bucket on eventFilter
    return true;
  }

  eventFilter[eventName] = bucket;                        // update bucket on eventFilter
  return false;
}

unsigned long SinricProDevice::getTimestamp() {
  if (eventSender) return eventSender->getTimestamp();
  return 0;
}

template <typename T>
T SinricProDevice::limitValue(T value, T minValue, T maxValue) {
  T newValue = value;
  if (value > maxValue) newValue = maxValue;
  if (value < minValue) newValue = minValue;
  return newValue;
}

/**
 * @brief Set callback function for `powerState` request
 * 
 * @param cb Function pointer to a `PowerStateCallback` function
 * @return void
 * @see PowerStateCallback
 **/
void SinricProDevice::onPowerState(PowerStateCallback cb) { 
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
bool SinricProDevice::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  return sendEvent(eventMessage);
}

String SinricProDevice::getProductType()  { 
  return String("sinric.device.type."); 
}

#endif