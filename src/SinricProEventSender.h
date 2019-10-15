/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _EVENTSENDER_H_
#define _EVENTSENDER_H_

#include "ArduinoJson.h"
#include "SinricProQueue.h"

class EventSender {
  public:
    virtual void sendEvent(JsonDocument& jsonEvent);
    virtual DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause);
};


#endif