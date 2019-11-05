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

class SinricProDevice_t : public SinricProDeviceInterface {
  public:
    SinricProDevice_t(const char* newDeviceId, unsigned long eventWaitTime=100);
    SinricProDevice_t(const SinricProDevice_t&) = delete;
    virtual ~SinricProDevice_t();
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
typedef SinricProDevice_t& SinricProDevice;

SinricProDevice_t::SinricProDevice_t(const char* newDeviceId, unsigned long eventWaitTime) : 
  eventSender(nullptr),
  eventWaitTime(eventWaitTime) {
  deviceId = strdup(newDeviceId);
  if (this->eventWaitTime < 100) this->eventWaitTime = 100;
}

SinricProDevice_t::~SinricProDevice_t() {
  if (deviceId) free(deviceId);
}

void SinricProDevice_t::begin(SinricProInterface* eventSender) {
  this->eventSender = eventSender;
}

const char* SinricProDevice_t::getDeviceId() {
  return deviceId;
}

DynamicJsonDocument SinricProDevice_t::prepareEvent(const char* deviceId, const char* action, const char* cause) {
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
  return DynamicJsonDocument(1024);
}

bool SinricProDevice_t::sendEvent(JsonDocument& event) {
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