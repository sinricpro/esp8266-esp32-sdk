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