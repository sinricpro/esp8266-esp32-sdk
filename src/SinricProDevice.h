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
    virtual const char* getDeviceId();
    virtual void begin(SinricProInterface* eventSender);
    virtual void setEventWaitTime(unsigned long eventWaitTime) { if (eventWaitTime<100) {this->eventWaitTime=100;} else { this->eventWaitTime=eventWaitTime;} }

    // standard request handler
    virtual bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value);

    // standard callbacks
	  typedef std::function<bool(const String, bool&)> PowerStateCallback;
    virtual void onPowerState(PowerStateCallback cb) { powerStateCallback = cb; }

    // standard events
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool sendEvent(JsonDocument& event);
    virtual DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause);
    char* deviceId;
    PowerStateCallback powerStateCallback;
  private:
    SinricProInterface* eventSender;
    unsigned long eventWaitTime;
    std::map<String, unsigned long> eventFilter;
};

SinricProDevice::SinricProDevice(const char* newDeviceId, unsigned long eventWaitTime) : 
  powerStateCallback(nullptr),
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

bool SinricProDevice::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  DEBUG_SINRIC("SinricProDevice::handleRequest()\r\n");
  bool success = false;
  String actionString = String(action);

  if (actionString == "setPowerState" && powerStateCallback) {
    bool powerState = request_value["state"]=="On"?true:false;
    success = powerStateCallback(String(deviceId), powerState);
    response_value["state"] = powerState?"On":"Off";
    return success;
  }
  return success;
}

DynamicJsonDocument SinricProDevice::prepareEvent(const char* deviceId, const char* action, const char* cause) {
  if (eventSender) return eventSender->prepareEvent(deviceId, action, cause);
  DEBUG_SINRIC("[SinricProDevice:prepareEvent()]: Device \"%s\" isn't configured correctly! The \'%s\' event will be ignored.\r\n", deviceId, action);
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

bool SinricProDevice::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  return sendEvent(eventMessage);
}

#endif