#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using LockStateCallback = std::function<bool(const String &, bool &)>;

template <typename T>
class LockController {
  public:
    LockController();

    void onLockState(LockStateCallback cb);
    bool sendLockStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onLockState(bool &lockState);
    bool handleLockController(SinricProRequest &request);
  
  private:
    EventLimiter event_limiter;
    LockStateCallback lockStateCallback;
};

template <typename T>
LockController<T>::LockController() 
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&LockController<T>::handleLockController, this, std::placeholders::_1)); 
}

template <typename T>
void LockController<T>::onLockState(LockStateCallback cb) {
  lockStateCallback = cb;
}

template <typename T>
bool LockController<T>::onLockState(bool &lockState) {
  T* device = static_cast<T*>(this);
  if (lockStateCallback) return lockStateCallback(device->deviceId, lockState);
  return false;
}

template <typename T>
bool LockController<T>::sendLockStateEvent(bool state, String cause) {
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setLockState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  state ? event_value["state"] = "LOCKED" : event_value["state"] = "UNLOCKED";
  return device->sendEvent(eventMessage);
}

template <typename T>
bool LockController<T>::handleLockController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setLockState")  {
    bool lockState = request.request_value["state"] == "lock" ? true : false;
    success = onLockState(lockState);
    request.response_value["state"] = success ? lockState ? "LOCKED" : "UNLOCKED" : "JAMMED";
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE