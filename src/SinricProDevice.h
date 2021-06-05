/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include <map>
#include "SinricProRequest.h"
#include "SinricProDeviceInterface.h"
#include "SinricProId.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

class SinricProDevice : public SinricProDeviceInterface {
  friend class SinricProClass;
public:
                              SinricProDevice(const DeviceId &deviceId, const String &productType = "CUSTOM");
  virtual                     ~SinricProDevice();
  bool                        operator==(const DeviceId& other) const;

  virtual const DeviceId      getDeviceId() const;
  virtual const String        getProductType() const;
protected:
  void                        registerRequestHandler(const SinricProRequestHandler &requestHandler);
  const unsigned long         getTimestamp() const;
  virtual bool                sendEvent(JsonDocument &event);
  virtual DynamicJsonDocument prepareEvent(const char *action, const char *cause);

  virtual void                loop() {};

  bool                        handleRequest(SinricProRequest &request);
  DeviceId deviceId;
  std::vector<SinricProRequestHandler> requestHandlers;

private: 
  String productType;
};

SinricProDevice::SinricProDevice(const DeviceId &deviceId, const String &productType) 
: deviceId(deviceId)
, productType(productType) {
  SinricPro.registerDevice(this);
}

SinricProDevice::~SinricProDevice() {
  SinricPro.unregisterDevice(this);
}

const DeviceId SinricProDevice::getDeviceId() const {
  return deviceId;
}

bool SinricProDevice::operator==(const DeviceId &other) const { 
  return other == deviceId; 
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* action, const char* cause) {
  return SinricPro.prepareEvent(deviceId, action, cause);
}

bool SinricProDevice::sendEvent(JsonDocument& event) {
  if (!SinricPro.isConnected()) {
    DEBUG_SINRIC("[SinricProDevice::sendEvent]: The event could not be sent. No connection to the SinricPro server.\r\n");
    return false;
  }

  SinricPro.sendMessage(event);
  return true;
}

const unsigned long SinricProDevice::getTimestamp() const {
  return SinricPro.getTimestamp();
}

void SinricProDevice::registerRequestHandler(const SinricProRequestHandler &requestHandler) {
  requestHandlers.push_back(requestHandler);
}

const String SinricProDevice::getProductType() const { 
  return String("sinric.device.type.")+productType; 
}

bool SinricProDevice::handleRequest(SinricProRequest &request) {
  for (auto& requestHandler : requestHandlers) {
    if (requestHandler(request)) return true;
  }
  return false;
}

} // SINRICPRO_NAMESPACE