/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDEVICE_H_
#define _SINRICDEVICE_H_

#include "SinricProDeviceInterface.h"
#include <map>

class SinricProDevice : public SinricProDeviceInterface {
  public:
    SinricProDevice(const char* newDeviceId, unsigned long eventWaitTime=100);
    virtual ~SinricProDevice();
    virtual bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) { return false; };
    virtual const char* getDeviceId();
    virtual void begin(SinricProInterface* eventSender);
  protected:
    virtual bool sendEvent(JsonDocument& event);
    virtual DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause);
    char* deviceId;
  private:
    SinricProInterface* eventSender;
    unsigned long eventWaitTime;
    std::map<String, unsigned long> eventFilter;
};

SinricProDevice::SinricProDevice(const char* newDeviceId, unsigned long eventWaitTime) : 
  eventSender(nullptr),
  eventWaitTime(eventWaitTime) {
  deviceId = strdup(newDeviceId);
  if (this->eventWaitTime < 100) this->eventWaitTime = 100;
}

SinricProDevice::~SinricProDevice() {
  if (deviceId) free(deviceId);
}

void SinricProDevice::begin(SinricProInterface* eventSender) {
  this->eventSender = eventSender;
}

const char* SinricProDevice::getDeviceId() {
  return deviceId;
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* deviceId, const char* action, const char* cause) {
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
  DEBUG_SINRIC("[SinricProDevice:prepareEvent()]: Device \"%s\" isn't configured correctly! The event is not sent.\r\n", deviceId);
  return DynamicJsonDocument(1024);
}

bool SinricProDevice::sendEvent(JsonDocument& event) {
  unsigned long actualMillis = millis();
  String eventName = event["payload"]["action"] | ""; // get event name

  if (eventFilter.find(eventName) == eventFilter.end()) {  // if eventFilter is not initialized
    eventFilter[eventName] = -eventWaitTime; // initialize eventFilter
  }

  unsigned long lastEventMillis = eventFilter[eventName] | 0; // get the last timestamp for event
  if (actualMillis - lastEventMillis < eventWaitTime) return false; // if last event was before waitTime return...

  if (eventSender) eventSender->sendMessage(event); // send event
  eventFilter[eventName] = actualMillis; // update lastEventTime
  return true;
}
#endif