#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using GenericToggleStateCallback = std::function<bool(const String &, const String&, bool &)>;

template <typename T>
class ToggleController {
  public:
    ToggleController();

    void onToggleState(const String& instance, GenericToggleStateCallback cb);
    bool sendToggleStateEvent(const String &instance, bool state, String cause = "PHYSICAL_INTERACTION");

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

  DynamicJsonDocument eventMessage = device->prepareEvent("setToggleState", cause.c_str());
  eventMessage["payload"]["instanceId"] = instance;
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["state"] = state ? "On" : "Off";
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ToggleController<T>::handleToggleController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setToggleState")  {
    bool state = request.request_value["state"] == "On" ? true : false;
    success = onToggleState(request.instance, state);
    request.response_value["state"] = state ? "On" : "Off";
    return success;
  }
  return success;
}


} // SINRICPRO_NAMESPACE