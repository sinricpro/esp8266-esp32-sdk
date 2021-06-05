#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using DoorCallback = std::function<bool(const String &, bool &)>;

template <typename T>
class DoorController {
  public:
    DoorController();

    void onDoorState(DoorCallback cb);
    bool sendDoorStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");
  protected:
    virtual bool onDoorState(bool &state);
    
    bool handleDoorController(SinricProRequest &request);
  
  private:
    EventLimiter event_limiter;
    DoorCallback doorCallback;
};

template <typename T>
DoorController<T>::DoorController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&DoorController<T>::handleDoorController, this, std::placeholders::_1)); 
}

template <typename T>
void DoorController<T>::onDoorState(DoorCallback cb) {
  doorCallback = cb;
}

template <typename T>
bool DoorController<T>::onDoorState(bool &state) {
  T* device = static_cast<T*>(this);
  if (doorCallback) return doorCallback(device->deviceId, state);
  return false;
}

template <typename T>
bool DoorController<T>::sendDoorStateEvent(bool state, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["mode"] = "Close" : event_value["mode"] = "Open";
  return device->sendEvent(eventMessage);
}

template <typename T>
bool DoorController<T>::handleDoorController(SinricProRequest &request) {
  bool success = false;
  if (request.action == "setMode") {
    String mode = request.request_value["mode"] | "";
    bool state = mode == "Close";
    success = onDoorState(state);
    request.response_value["mode"] = state ? "Close" : "Open";
  }
  return success;
}


} // SINRICPRO_NAMESPACE