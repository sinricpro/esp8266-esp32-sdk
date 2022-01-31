#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(POWERSTATE, state);             // "state"
FSTR(POWERSTATE, On);                // "On"
FSTR(POWERSTATE, Off);               // "Off"
FSTR(POWERSTATE, setPowerState);     // "setPowerState"

/**
 * @brief Callback definition for onPowerState function
 * 
 * Gets called when device receive a `setPowerState` reuqest \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   state       `true` = device is requested to turn on \n `false` = device is requested to turn off
 * @param[out]  state       `true` = device has been turned on \n `false` = device has been turned off
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * @section PowerStateCallback Example-Code
 * @snippet callbacks.cpp onPowerState
 **/
using PowerStateCallback = std::function<bool(const String &, bool &)>;


/**
 * @brief PowerStateController
 * @ingroup Capabilities
 **/
template <typename T>
class PowerStateController {
  public:
    PowerStateController();

    void onPowerState(PowerStateCallback cb);
    bool sendPowerStateEvent(bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handlePowerStateController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    PowerStateCallback powerStateCallback;
};

template <typename T>
PowerStateController<T>::PowerStateController() 
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&PowerStateController<T>::handlePowerStateController, this, std::placeholders::_1));
}

/**
 * @brief Set callback function for `powerState` request
 * 
 * @param cb Function pointer to a `PowerStateCallback` function
 * @return void
 * @see PowerStateCallback
 **/
template <typename T>
void PowerStateController<T>::onPowerState(PowerStateCallback cb) {
  powerStateCallback = cb;
}

/**
 * @brief Send `setPowerState` event to SinricPro Server indicating actual power state
 * 
 * @param state   `true` = device turned on \n `false` = device turned off
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
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
  T* device = static_cast<T*>(this);

  bool success = false;

  if (request.action == FSTR_POWERSTATE_setPowerState && powerStateCallback)  {
    bool powerState = request.request_value[FSTR_POWERSTATE_state] == FSTR_POWERSTATE_On ? true : false;
    success = powerStateCallback(device->deviceId, powerState);
    request.response_value[FSTR_POWERSTATE_state] = powerState ? FSTR_POWERSTATE_On : FSTR_POWERSTATE_Off;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using PowerStateController = SINRICPRO_NAMESPACE::PowerStateController<T>;