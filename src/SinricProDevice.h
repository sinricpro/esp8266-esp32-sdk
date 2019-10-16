/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICDEVICE_H_
#define _SINRICDEVICE_H_

#include "SinricProEventSender.h"
#include <map>

class SinricProDevice {
  public:
    SinricProDevice(const char* newDeviceId, unsigned long eventWaitTime=100);
    virtual ~SinricProDevice();
    virtual bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) { return false; };
    const char* getDeviceId();
    void begin(EventSender* eventSender);
  protected:
    char* deviceId;
    bool sendEvent(JsonDocument& event);
    DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause);
  private:
    EventSender* eventSender;
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

void SinricProDevice::begin(EventSender* eventSender) {
  this->eventSender = eventSender;
}

const char* SinricProDevice::getDeviceId() {
  return deviceId;
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* deviceId, const char* action, const char* cause) {
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
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

  if (eventSender) eventSender->sendEvent(event); // send event
  eventFilter[eventName] = actualMillis; // update lastEventTime
  return true;
}
#endif