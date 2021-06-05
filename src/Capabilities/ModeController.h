#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using ModeCallback = std::function<bool(const String &, String &)>;
using GenericModeCallback = std::function<bool(const String &, const String &, String &)>;

template <typename T>
class ModeController {
  public:
    ModeController();

    void onSetMode(ModeCallback cb);
    void onSetMode(const String& instance, GenericModeCallback cb);
    bool sendModeEvent(String mode, String cause = "PHYSICAL_INTERACTION");
    bool sendModeEvent(String instance, String mode, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onSetMode(String& mode);
    virtual bool onSetMode(const String& instance, String &mode);
    bool handleModeController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    std::map<String, EventLimiter> generic_event_limiter;
    ModeCallback setModeCallback;
    std::map<String, GenericModeCallback> genericModeCallback;
};

template <typename T>
ModeController<T>::ModeController() 
: event_limiter(EVENT_LIMIT_STATE) { 
  T *device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&ModeController<T>::handleModeController, this, std::placeholders::_1)); 
}

template <typename T>
void ModeController<T>::onSetMode(ModeCallback cb) {
  setModeCallback = cb;
}

template <typename T>
void ModeController<T>::onSetMode(const String& instance, GenericModeCallback cb) {
  genericModeCallback[instance] = cb;
}

template <typename T>
bool ModeController<T>::onSetMode(String& mode) {
  T *device = static_cast<T*>(this);
  if (setModeCallback) return setModeCallback(device->deviceId, mode);
  return false;  
}

template <typename T>
bool ModeController<T>::onSetMode(const String& instance, String &mode) {
  T *device = static_cast<T*>(this);
  if (genericModeCallback[instance]) return genericModeCallback[instance](device->deviceId, instance, mode);
  return false;
}

template <typename T>
bool ModeController<T>::sendModeEvent(String mode, String cause) {
  if (event_limiter) return false;
  T *device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mode"] = mode;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ModeController<T>::sendModeEvent(String instance, String mode, String cause) {
  if (generic_event_limiter.find(instance) == generic_event_limiter.end()) generic_event_limiter[instance] = EventLimiter(EVENT_LIMIT_STATE);
  if (generic_event_limiter[instance]) return false;
  T *device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setMode", cause.c_str());
  eventMessage["payload"]["instanceId"] = instance;
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mode"] = mode;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ModeController<T>::handleModeController(SinricProRequest &request) {
  bool success = false;
  if (request.action != "setMode") return false;
  String mode = request.request_value["mode"] | "";

  if (request.instance != "") {
    success = onSetMode(request.instance, mode);
    request.response_value["mode"] = mode;
    return success;

  } else {
    success = onSetMode(mode);
    request.response_value["mode"] = mode;
    return success;
  }
  
  return success;
}

}// SINRICPRO_NAMESPACE