#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(LOCK, setLockState);      // "setLockState"
FSTR(LOCK, state);             // "state"
FSTR(LOCK, LOCKED);            // "LOCKED"
FSTR(LOCK, UNLOCKED);          // "UNLOCKED"
FSTR(LOCK, lock);              // "lock"
FSTR(LOCK, JAMMED);            // "JAMMED"

/**
 * @brief Callback definition for onLockState function
 * 
 * Gets called when device receive a `setLockState` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   state       `true` = device is requested to lock \n `false` = device is requested to unlock
 * @param[out]  state       `true` = device has been locked \n `false` = device has been unlocked
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section LockStateCallback Example-Code
 * @code
 * bool onLockState(const String &deviceId, bool &lockState) {
 *   Serial.printf("Device is %s\r\n", lockState?"locked":"unlocked");
 *   return true;
 * }
 * @endcode
 **/
using LockStateCallback = std::function<bool(const String &, bool &)>; // void onLockState(const DeviceId &deviceId, bool& lockState);


/**
 * @brief LockController
 * @ingroup Capabilities
 **/
template <typename T>
class LockController {
  public:
    LockController();

    void onLockState(LockStateCallback cb);
    bool sendLockStateEvent(bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
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

/**
 * @brief Set callback function for `setLockState` request
 * 
 * @param cb Function pointer to a `LockStateCallback` function
 * @return void
 * @see LockStateCallback
 **/
template <typename T>
void LockController<T>::onLockState(LockStateCallback cb) {
  lockStateCallback = cb;
}

/**
 * @brief Send `lockState` event to SinricPro Server indicating actual lock state
 * 
 * @param state   `true` = device is locked \n `false` = device is unlocked
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool LockController<T>::sendLockStateEvent(bool state, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_LOCK_setLockState, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  state ? event_value[FSTR_LOCK_state] = FSTR_LOCK_LOCKED : event_value[FSTR_LOCK_state] = FSTR_LOCK_UNLOCKED;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool LockController<T>::handleLockController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (request.action == FSTR_LOCK_setLockState && lockStateCallback)  {
    bool lockState = request.request_value[FSTR_LOCK_state] == FSTR_LOCK_lock ? true : false;
    success = lockStateCallback(device->deviceId, lockState);
    request.response_value[FSTR_LOCK_state] = success ? lockState ? FSTR_LOCK_LOCKED : FSTR_LOCK_UNLOCKED : FSTR_LOCK_JAMMED;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using LockController = SINRICPRO_NAMESPACE::LockController<T>;