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
    SinricProDevice(const char* newDeviceId, unsigned long eventWaitTime=100);
    virtual ~SinricProDevice();
    virtual const char* getDeviceId();
    virtual void begin(SinricProInterface* eventSender);
    virtual void setEventWaitTime(unsigned long eventWaitTime) { if (eventWaitTime<100) {this->eventWaitTime=100;} else { this->eventWaitTime=eventWaitTime;} }

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
    virtual bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value);

    // standard Callbacks
    virtual void onPowerState(PowerStateCallback cb);

    // standard events
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool sendEvent(JsonDocument& event);
    virtual DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause);
    unsigned long getTimestamp();
    char* deviceId;
    PowerStateCallback powerStateCallback;
  private:
    SinricProInterface* eventSender;
    unsigned long eventWaitTime;
    std::map<String, LeakyBucket_t> eventFilter;
};

SinricProDevice::SinricProDevice(const char* newDeviceId, unsigned long eventWaitTime) : 
  powerStateCallback(nullptr),
  eventSender(nullptr),
  eventWaitTime(eventWaitTime) {
  deviceId = strdup(newDeviceId);
  if (this->eventWaitTime < 100) this->eventWaitTime = 100;
}

SinricProDevice::~SinricProDevice() {
  if (deviceId) free(deviceId);
}

void SinricProDevice::begin(SinricProInterface* eventSender) {
  this->eventSender = eventSender;
}

const char* SinricProDevice::getDeviceId() {
  return deviceId;
}

bool SinricProDevice::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  DEBUG_SINRIC("SinricProDevice::handleRequest()\r\n");
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(String(deviceId), powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }
  return success;
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* deviceId, const char* action, const char* cause) {
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
  DEBUG_SINRIC("[SinricProDevice:prepareEvent()]: Device \"%s\" isn't configured correctly! The \'%s\' event will be ignored.\r\n", deviceId, action);
  return DynamicJsonDocument(1024);
}


bool SinricProDevice::sendEvent(JsonDocument& event) {
  String eventName = event["payload"]["action"] | ""; // get event name

  LeakyBucket_t bucket; // leaky bucket algorithm is used to prevent flooding the server

  // get leaky bucket for event from eventFilter
  if (eventFilter.find(eventName) == eventFilter.end()) {  // if there is no bucket ...
    eventFilter[eventName] = bucket;                       // ...add a new bucket
  } else {  
    bucket = eventFilter[eventName];                       // else get bucket
  }

  if (bucket.addDrop()) {                                  // if we can add a new drop
    if (eventSender) eventSender->sendMessage(event);      // send event
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

#endif