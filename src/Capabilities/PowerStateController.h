#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @ingroup Callbacks
 * @param deviceId `const String&`  deviceId
 * @param state    `bool&`          state (`true`: on / `false`:off)
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
    bool sendPowerStateEvent(bool state, String cause = "PHYSICAL_INTERACTION");

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

  DynamicJsonDocument eventMessage = device->prepareEvent("setPowerState", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "On" : "Off";
  return device->sendEvent(eventMessage);
}

template <typename T>
bool PowerStateController<T>::handlePowerStateController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setPowerState")  {
    bool powerState = request.request_value["state"] == "On" ? true : false;
    success = onPowerState(powerState);
    request.response_value["state"] = powerState ? "On" : "Off";
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE