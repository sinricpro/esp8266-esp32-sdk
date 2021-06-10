/**
 * @file ToggleController.h
 * @author Boris Jäger (boris.jaeger@sinric.com)
 * @brief 
 * @version 3.0.0
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(TOGGLE, setToggleState);
FSTR(TOGGLE, state);
FSTR(TOGGLE, On);
FSTR(TOGGLE, Off);

/**
 * @ingroup Callbacks
 * @param deviceId `const String&`  deviceId
 * @param instance `const String&`  instanceId
 * @param state    `bool&`          state (`true`: on / `false`:off)
 * @return true     request was handled
 * @return false    request could not be handled
 * #### Example
 * @include callbacks/onToggleState.cpp
 */
using GenericToggleStateCallback = std::function<bool(const String &, const String&, bool &)>;

/**
 * @brief ToggleController
 * @ingroup Capabilities
 */
template <typename T>
class ToggleController {
  public:
    ToggleController();

    void onToggleState(const String& instance, GenericToggleStateCallback cb);
    bool sendToggleStateEvent(const String &instance, bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    virtual bool onToggleState(const String& instance, bool &state);
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
 * @brief Set onToggleStateCallback 
 * @param instance The InstanceID to which the callback should be attached.
 * @param cb       Function pointer to a @ref GenericToggleStateCallback
 */
template <typename T>
void ToggleController<T>::onToggleState(const String& instance, GenericToggleStateCallback cb) {
  genericToggleStateCallback[instance] = cb;
}

template <typename T>
bool ToggleController<T>::onToggleState(const String& instance, bool &state) {
  T* device = static_cast<T*>(this);
  if (genericToggleStateCallback[instance]) genericToggleStateCallback[instance](device->deviceId, instance, state);
  return false;  
}

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
  bool success = false;

  if (request.action == FSTR_TOGGLE_setToggleState)  {
    bool state = request.request_value[FSTR_TOGGLE_state] == FSTR_TOGGLE_On ? true : false;
    success = onToggleState(request.instance, state);
    request.response_value[FSTR_TOGGLE_state] = state ? FSTR_TOGGLE_On : FSTR_TOGGLE_Off;
    return success;
  }
  return success;
}


} // SINRICPRO_NAMESPACE