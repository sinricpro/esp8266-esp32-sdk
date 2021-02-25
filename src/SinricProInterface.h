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
#include "SinricProId.h"

class SinricProInterface {
  friend class SinricProDevice;
  protected:
    virtual void sendMessage(JsonDocument& jsonEvent);
    virtual DynamicJsonDocument prepareEvent(DeviceId deviceId, const char* action, const char* cause);
    virtual unsigned long getTimestamp(); 
    virtual bool isConnected();
};


#endif