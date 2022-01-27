/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "ArduinoJson.h"
#include "SinricProQueue.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

class SinricProInterface {
  friend class SinricProDevice;
  protected:
    virtual void                sendMessage(JsonDocument& jsonEvent) = 0;
    virtual DynamicJsonDocument prepareEvent(String deviceId, const char* action, const char* cause) = 0;
    virtual unsigned long       getTimestamp() = 0; 
    virtual bool                isConnected() = 0;
};

} // SINRICPRO_NAMESPACE