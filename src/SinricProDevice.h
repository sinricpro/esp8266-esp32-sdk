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

#define BUCKET_SIZE 20
#define DROP_WAIT_TIME 30000
#define DROP_ADD_FACTOR 100

struct leackyBucket_t {
  int dropsInBucket=0;
  unsigned long lastDrop;
};

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
	  typedef std::function<bool(const String&, bool&)> PowerStateCallback;
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
    std::map<String, leackyBucket_t> eventFilter;
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

  leackyBucket_t bucket;

// get Bucket for eventName

  if (eventFilter.find(eventName) == eventFilter.end()) {  // if eventFilter is not initialized
//    eventFilter[eventName] = -eventWaitTime; // initialize eventFilter
    bucket.dropsInBucket = 0;
    bucket.lastDrop = -eventWaitTime;
    eventFilter[eventName] = bucket;
  } else {  // if eventFilter is initialized, get bucket
    bucket = eventFilter[eventName]; 
  }
  DEBUG_SINRIC("Bucket dropsInBucket: %d\r\n", bucket.dropsInBucket);
// leack bucket...
  int drops_to_leak = (actualMillis - bucket.lastDrop) / DROP_WAIT_TIME;
  DEBUG_SINRIC("Bucket leaking: %d\r\n", drops_to_leak);
  if (drops_to_leak > 0) {
    if (bucket.dropsInBucket <= drops_to_leak) {
      bucket.dropsInBucket = 0;
    } else {
      bucket.dropsInBucket -= drops_to_leak;
    }
  }

//  unsigned long lastEventMillis = eventFilter[eventName] | 0; // get the last timestamp for event
//  if (actualMillis - lastEventMillis < eventWaitTime) return false; // if last event was before waitTime return...
//  if (actualMillis - bucket.lastDrop < eventWaitTime) return false; 
  if (bucket.dropsInBucket < BUCKET_SIZE && actualMillis-bucket.lastDrop > bucket.dropsInBucket * DROP_ADD_FACTOR) { // new drop can be placed into bucket?
    Serial.printf("SinricProDevice::sendMessage(): %d event's left before limiting to 1 event per %d seconds. %lu ms until next event\r\n", BUCKET_SIZE-bucket.dropsInBucket-1, DROP_WAIT_TIME / 1000, ((bucket.dropsInBucket+1) * DROP_ADD_FACTOR));
    bucket.dropsInBucket++; // place drop in bucket
    bucket.lastDrop = actualMillis; // store last drop time
    eventFilter[eventName] = bucket; // save bucket back to map
    if (eventSender) eventSender->sendMessage(event); // send event
    return true;
  }

  if (bucket.dropsInBucket >= BUCKET_SIZE) {
    Serial.printf("- WARNING: EVENTS ARE BLOCKED FOR %lu SECONDS (%lu seconds left)\r", DROP_WAIT_TIME/1000, (DROP_WAIT_TIME-(actualMillis-bucket.lastDrop))/1000);
  }
  return false;
}

bool SinricProDevice::sendPowerStateEvent(bool state, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state?"On":"Off";
  return sendEvent(eventMessage);
}

#endif