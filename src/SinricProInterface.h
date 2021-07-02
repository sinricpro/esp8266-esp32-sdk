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
    virtual void                sendMessage(JsonDocument& jsonEvent);
    virtual DynamicJsonDocument prepareEvent(String deviceId, const char* action, const char* cause);
    virtual unsigned long       getTimestamp(); 
    virtual bool                isConnected();
};

} // SINRICPRO_NAMESPACE