#ifndef _SINRICDEVICE_H_
#define _SINRICDEVICE_H_

#include "EventSender.h"
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
    void sendEvent(JsonDocument& event);
    DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause);
  private:
    EventSender* eventSender;
    unsigned long lastEventMillis;
    unsigned long eventWaitTime;
    std::map<String, unsigned long> eventFilter;
};

SinricProDevice::SinricProDevice(const char* newDeviceId, unsigned long eventWaitTime) : 
  eventSender(nullptr),
  lastEventMillis(0),
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

void SinricProDevice::sendEvent(JsonDocument& event) {
  unsigned long actualMillis = millis();
/*
  if (actualMillis - lastEventMillis < eventWaitTime) return;
  if (eventSender) eventSender->sendEvent(event);
  lastEventMillis = actualMillis;
*/
  String eventName = event["payload"]["action"] | ""; // get event name
  unsigned long lastEventMillis = eventFilter[eventName] | 0; // get the last timestamp for event
  if (actualMillis - lastEventMillis < eventWaitTime) return; // if last event was before waitTime return...

  if (eventSender) eventSender->sendEvent(event); // send event
  eventFilter[eventName] = actualMillis; // set lastEventTime to now
}
#endif