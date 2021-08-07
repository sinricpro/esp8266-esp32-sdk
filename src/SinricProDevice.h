/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include <map>

#include "SinricPro.h"
#include "SinricProDeviceInterface.h"
#include "SinricProNamespace.h"
#include "SinricProRequest.h"
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
    virtual ~SinricProDevice();
    bool operator==(const String &other);

    virtual String getDeviceId();

  protected:
    void registerRequestHandler(const SinricProRequestHandler &requestHandler);
    virtual void loop();
    unsigned long getTimestamp();
    virtual bool sendEvent(JsonDocument &event);
    virtual DynamicJsonDocument prepareEvent(const char *action, const char *cause);

    virtual String getProductType();
    bool handleRequest(SinricProRequest &request);

    String deviceId;
    std::vector<SinricProRequestHandler> requestHandlers;

  private:
    String productType;
};

SinricProDevice::SinricProDevice(const String &deviceId, const String &productType)
    : deviceId(deviceId)
    , productType(productType) {
    SinricPro.registerDevice(this);
}

SinricProDevice::~SinricProDevice() {
    SinricPro.unregisterDevice(this);
}

String SinricProDevice::getDeviceId() {
    return deviceId;
}

bool SinricProDevice::operator==(const String &other) {
    return other == deviceId;
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char *action, const char *cause) {
    return SinricPro.prepareEvent(deviceId, action, cause);
}

bool SinricProDevice::sendEvent(JsonDocument &event) {
    if (!SinricPro.isConnected()) {
        DEBUG_SINRIC("[SinricProDevice::sendEvent]: The event could not be sent. No connection to the SinricPro server.\r\n");
        return false;
    }

    SinricPro.sendMessage(event);
    return true;
}

void SinricProDevice::registerRequestHandler(const SinricProRequestHandler &requestHandler) {
    requestHandlers.push_back(requestHandler);
}

void SinricProDevice::loop() {
    // does nothing, must be overriden
}

unsigned long SinricProDevice::getTimestamp() {
    return SinricPro.getTimestamp();
}

String SinricProDevice::getProductType() {
    return String("sinric.device.type.") + productType;
}

bool SinricProDevice::handleRequest(SinricProRequest &request) {
    for (auto &requestHandler : requestHandlers) {
        if (requestHandler(request)) return true;
    }
    return false;
}

}  // namespace SINRICPRO_NAMESPACE