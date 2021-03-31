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
  SinricProDevice(const DeviceId &deviceId, const String &productType = "CUSTOM");
  bool operator==(const DeviceId& other);

  virtual DeviceId getDeviceId();
  virtual String getProductType();
protected:
  unsigned long getTimestamp();
  virtual bool sendEvent(JsonDocument &event);
  virtual DynamicJsonDocument prepareEvent(const char *action, const char *cause);
#if defined(SINRICPRO_OO)  
  virtual void loop() {};
#endif  

  virtual ~SinricProDevice();
#if !defined(SINRICPRO_OO)  
  virtual void begin(SinricProInterface *eventSender);
#endif  
  bool handleRequest(SinricProRequest &request);
  DeviceId deviceId;
  std::vector<SinricProRequestHandler> requestHandlers;

private : SinricProInterface *eventSender;
  std::map<String, LeakyBucket_t> eventFilter;
  String productType;
};

SinricProDevice::SinricProDevice(const DeviceId &deviceId, const String &productType) : 
  deviceId(deviceId),
#if !defined(SINRICPRO_OO)
  eventSender(nullptr),
#endif
  productType(productType) {
#if defined(SINRICPRO_OO)
  SinricPro.add(this);
#endif
}

SinricProDevice::~SinricProDevice() {
#if defined(SINRICPRO_OO)
  SinricPro.remove(this);
#endif
}

#if !defined(SINRICPRO_OO)
void SinricProDevice::begin(SinricProInterface* eventSender) {
  this->eventSender = eventSender;
}
#endif

DeviceId SinricProDevice::getDeviceId() {
  return deviceId;
}

bool SinricProDevice::operator==(const DeviceId &other) { 
  return other == deviceId; 
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* action, const char* cause) {
#if !defined(SINRICPRO_OO)  
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
  DEBUG_SINRIC("[SinricProDevice:prepareEvent()]: Device \"%s\" isn't configured correctly! The \'%s\' event will be ignored.\r\n", deviceId.toString().c_str(), action);
  return DynamicJsonDocument(1024);
#else
  return SinricPro.prepareEvent(deviceId, action, cause);
#endif
}

#if !defined(SINRICPRO_OO)
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

#else 

bool SinricProDevice::sendEvent(JsonDocument& event) {
  if (!SinricPro.isConnected()) {
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
    SinricPro.sendMessage(event);                       // send event
    eventFilter[eventName] = bucket;                       // update bucket on eventFilter
    return true;
  }

  eventFilter[eventName] = bucket;                        // update bucket on eventFilter
  return false;
}

#endif

unsigned long SinricProDevice::getTimestamp() {
#if !defined(SINRICPRO_OO)  
  if (eventSender) return eventSender->getTimestamp();
  return 0;
#else
  return SinricPro.getTimestamp();
#endif  
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