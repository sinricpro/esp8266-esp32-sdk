#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(LOCK, setLockState);
FSTR(LOCK, state);
FSTR(LOCK, LOCKED);
FSTR(LOCK, UNLOCKED);
FSTR(LOCK, lock);
FSTR(LOCK, JAMMED);

using LockStateCallback = std::function<bool(const String &, bool &)>;

template <typename T>
class LockController {
  public:
    LockController();

    void onLockState(LockStateCallback cb);
    bool sendLockStateEvent(bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

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

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_LOCK_setLockState, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  state ? event_value[FSTR_LOCK_state] = FSTR_LOCK_LOCKED : event_value[FSTR_LOCK_state] = FSTR_LOCK_UNLOCKED;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool LockController<T>::handleLockController(SinricProRequest &request) {
  bool success = false;

  if (request.action == FSTR_LOCK_setLockState)  {
    bool lockState = request.request_value[FSTR_LOCK_state] == FSTR_LOCK_lock ? true : false;
    success = onLockState(lockState);
    request.response_value[FSTR_LOCK_state] = success ? lockState ? FSTR_LOCK_LOCKED : FSTR_LOCK_UNLOCKED : FSTR_LOCK_JAMMED;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE