/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDEVICE_H_
#define _SINRICDEVICE_H_

#include "SinricProRequest.h"
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
  friend class SinricProClass;
public:
  SinricProDevice(const DeviceId &deviceId, const String &productType = "");
  bool operator==(const DeviceId& other);

  virtual DeviceId getDeviceId();
protected:
  unsigned long getTimestamp();
  virtual bool sendEvent(JsonDocument &event);
  virtual DynamicJsonDocument prepareEvent(const char *action, const char *cause);

  virtual ~SinricProDevice();
  virtual String getProductType();
  virtual void begin(SinricProInterface *eventSender);
  bool handleRequest(SinricProRequest &request);
  DeviceId deviceId;
  std::vector<SinricProRequestHandler> requestHandlers;

private : SinricProInterface *eventSender;
  std::map<String, LeakyBucket_t> eventFilter;
  String productType;
};

SinricProDevice::SinricProDevice(const DeviceId &deviceId, const String &productType) : 
  deviceId(deviceId),
  eventSender(nullptr),
  productType(productType) {
}

SinricProDevice::~SinricProDevice() {}

void SinricProDevice::begin(SinricProInterface* eventSender) {
  this->eventSender = eventSender;
}

DeviceId SinricProDevice::getDeviceId() {
  return deviceId;
}

bool SinricProDevice::operator==(const DeviceId &other) { 
  return other == deviceId; 
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* action, const char* cause) {
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
  String eventInstance = event["payload"]["instanceId"] | "";
  if (eventInstance != "") eventName += "_" + eventInstance;
  
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
  return String("sinric.device.type.")+productType; 
}

bool SinricProDevice::handleRequest(SinricProRequest &request) {
  for (auto& requestHandler : requestHandlers) {
    if (requestHandler(request)) return true;
  }
  return false;
}


#endif