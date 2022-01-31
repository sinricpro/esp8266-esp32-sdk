/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProRequest.h"
#include "SinricProDeviceInterface.h"
#include <map>

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

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
  SinricProDevice(const String &deviceId, const String &productType = "");
  bool                                 operator==(const String& other);

  virtual String                       getDeviceId();
protected:
  virtual                              ~SinricProDevice();

  void                                 registerRequestHandler(const SinricProRequestHandler &requestHandler);
  unsigned long                        getTimestamp();
  virtual bool                         sendEvent(JsonDocument &event);
  virtual DynamicJsonDocument          prepareEvent(const char *action, const char *cause);

  virtual String                       getProductType();
  virtual void                         begin(SinricProInterface *eventSender);
  bool                                 handleRequest(SinricProRequest &request);

  String                               deviceId;
  std::vector<SinricProRequestHandler> requestHandlers;

private:
  SinricProInterface                   *eventSender;
  String                               productType;
};

SinricProDevice::SinricProDevice(const String &deviceId, const String &productType) : 
  deviceId(deviceId),
  eventSender(nullptr),
  productType(productType) {
}

SinricProDevice::~SinricProDevice() {}

void SinricProDevice::begin(SinricProInterface* eventSender) {
  this->eventSender = eventSender;
}

String SinricProDevice::getDeviceId() {
  return deviceId;
}

bool SinricProDevice::operator==(const String &other) { 
  return other == deviceId; 
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* action, const char* cause) {
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
  DEBUG_SINRIC("[SinricProDevice:prepareEvent()]: Device \"%s\" isn't configured correctly! The \'%s\' event will be ignored.\r\n", deviceId.c_str(), action);
  return DynamicJsonDocument(1024);
}


bool SinricProDevice::sendEvent(JsonDocument& event) {
  if (!SinricPro.isConnected()) {
    DEBUG_SINRIC("[SinricProDevice::sendEvent]: The event could not be sent. No connection to the SinricPro server.\r\n");
    return false;
  }

  if (eventSender) {
    eventSender->sendMessage(event);
    return true;
  }
  
  return false;
}

void SinricProDevice::registerRequestHandler(const SinricProRequestHandler &requestHandler) {
  requestHandlers.push_back(requestHandler);
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

} // SINRICPRO_NAMESPACE

using SinricProDevice = SINRICPRO_NAMESPACE::SinricProDevice;