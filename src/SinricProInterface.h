/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICPRO_INTERFACE_H_
#define _SINRICPRO_INTERFACE_H_

#include "ArduinoJson.h"
#include "SinricProQueue.h"

class SinricProInterface {
  public:
    virtual void sendMessage(JsonDocument& jsonEvent);
    virtual DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause);
    virtual unsigned long getTimestamp(); 
};


#endif