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

    virtual bool sendEvent(JsonDocument &event);
    virtual DynamicJsonDocument prepareEvent(const DeviceId &deviceId, const char *action, const char *cause);

  protected:
    unsigned long getTimestamp();
    DeviceId deviceId;
  private:
    SinricProInterface* eventSender;
    std::map<String, LeakyBucket_t> eventFilter;
};

SinricProDevice::SinricProDevice(const DeviceId &deviceId) : 
  deviceId(deviceId),
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

String SinricProDevice::getProductType()  { 
  return String("sinric.device.type."); 
}

#endif