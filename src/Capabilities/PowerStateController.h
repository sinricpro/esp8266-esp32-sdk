#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(POWERSTATE, state);
FSTR(POWERSTATE, On);
FSTR(POWERSTATE, Off);
FSTR(POWERSTATE, setPowerState);

/**
 * @ingroup Callbacks
 * @param deviceId `const String&`  deviceId
 * @param[in] state true:on / false:off
 * @param[out] state true:device turned on / false:device turned off
 * @return true     request was handled
 * @return false    request could not be handled
 * #### Example
 * @include callbacks/onPowerState.cpp
 */
using PowerStateCallback = std::function<bool(const String &, bool &)>;

/**
 * @brief PowerStateController
 * @ingroup Capabilities
 */
template <typename T>
class PowerStateController {
  public:
    PowerStateController();

    void onPowerState(PowerStateCallback cb);
    bool sendPowerStateEvent(bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    virtual bool onPowerState(bool &state);
    bool handlePowerStateController(SinricProRequest &request);

  private:
    PowerStateCallback powerStateCallback;
    EventLimiter event_limiter;
};

template <typename T>
PowerStateController<T>::PowerStateController()
: event_limiter(EVENT_LIMIT_STATE) {
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&PowerStateController<T>::handlePowerStateController, this, std::placeholders::_1));
}

/**
 * @brief Set callback function for PowerState request
 * @param cb Function pointer to a @ref PowerStateCallback
 */
template <typename T>
void PowerStateController<T>::onPowerState(PowerStateCallback cb) {
  powerStateCallback = cb;
}

template <typename T>
bool PowerStateController<T>::onPowerState(bool &state) {
  T* device = static_cast<T*>(this);
  if (powerStateCallback) return powerStateCallback(device->deviceId, state);
  return false;
}

template <typename T>
bool PowerStateController<T>::sendPowerStateEvent(bool state, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_POWERSTATE_setPowerState, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_POWERSTATE_state] = state ? FSTR_POWERSTATE_On : FSTR_POWERSTATE_Off;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool PowerStateController<T>::handlePowerStateController(SinricProRequest &request) {
  bool success = false;

  if (request.action == FSTR_POWERSTATE_setPowerState)  {
    bool powerState = request.request_value[FSTR_POWERSTATE_state] == FSTR_POWERSTATE_On ? true : false;
    success = onPowerState(powerState);
    request.response_value[FSTR_POWERSTATE_state] = powerState ? FSTR_POWERSTATE_On : FSTR_POWERSTATE_Off;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE