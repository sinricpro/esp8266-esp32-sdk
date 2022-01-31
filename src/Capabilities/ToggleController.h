#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(TOGGLE, setToggleState);    // "setToggleState"
FSTR(TOGGLE, state);             // "state"
FSTR(TOGGLE, On);                // "On"
FSTR(TOGGLE, Off);               // "Off"

/**
   * @brief Callback definition for onToggleState function
   * 
   * Gets called when device receive a `setPowerState` reuqest \n
   * @param[in]   deviceId    String which contains the ID of device
   * @param[in]   instance    String which instance is requested
   * @param[in]   state       `true` = device is requested to turn on \n `false` = device is requested to turn off
   * @param[out]  state       `true` = device has been turned on \n `false` = device has been turned off
   * @return      the success of the request
   * @retval      true        request handled properly
   * @retval      false       request was not handled properly because of some error
   * @section ToggleStateCallback Example-Code
   * @snippet callbacks.cpp onToggleState
   **/
using GenericToggleStateCallback = std::function<bool(const String &, const String&, bool &)>;


/**
 * @brief ToggleController
 * @ingroup Capabilities
 **/
template <typename T>
class ToggleController {
  public:
    ToggleController();
  
    void onToggleState(const String& instance, GenericToggleStateCallback cb);
    bool sendToggleStateEvent(const String &instance, bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
  
  protected:
    bool handleToggleController(SinricProRequest &request);
  
  private:
    std::map<String, EventLimiter> event_limiter;
    std::map<String, GenericToggleStateCallback> genericToggleStateCallback;
};

template <typename T>
ToggleController<T>::ToggleController() { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&ToggleController<T>::handleToggleController, this, std::placeholders::_1)); 
}

/**
 * @brief Set callback function for `toggleState` request
 * 
 * @param instance String instance name (custom device)
 * @param cb Function pointer to a `ToggleStateCallback` function
 * @return void
 * @see ToggleStateCallback
 **/
template <typename T>
void ToggleController<T>::onToggleState(const String &instance, GenericToggleStateCallback cb) {
  genericToggleStateCallback[instance] = cb;
}

/**
 * @brief Send `setToggleState` event to SinricPro Server indicating actual toggle state
 * 
 * @param instance String instance name (custom device)
 * @param state   `true` = state turned on \n `false` = tate turned off
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ToggleController<T>::sendToggleStateEvent(const String &instance, bool state, String cause) {
  if (event_limiter.find(instance) == event_limiter.end()) event_limiter[instance] = EventLimiter(EVENT_LIMIT_STATE);
  if (event_limiter[instance]) return false;
  
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_TOGGLE_setToggleState, cause.c_str());
  eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId] = instance;
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_TOGGLE_state] = state ? FSTR_TOGGLE_On : FSTR_TOGGLE_Off;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ToggleController<T>::handleToggleController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (request.action == FSTR_TOGGLE_setToggleState)  {
    bool powerState = request.request_value[FSTR_TOGGLE_state] == FSTR_TOGGLE_On ? true : false;
    if (genericToggleStateCallback.find(request.instance) != genericToggleStateCallback.end())
      success = genericToggleStateCallback[request.instance](device->deviceId, request.instance, powerState);
    request.response_value[FSTR_TOGGLE_state] = powerState ? FSTR_TOGGLE_On : FSTR_TOGGLE_Off;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using ToggleController = SINRICPRO_NAMESPACE::ToggleController<T>;