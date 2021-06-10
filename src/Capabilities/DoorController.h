#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(DOOR, setMode);
FSTR(DOOR, mode);
FSTR(DOOR, Close);
FSTR(DOOR, Open);

using DoorCallback = std::function<bool(const String &, bool &)>;

template <typename T>
class DoorController {
  public:
    DoorController();

    void onDoorState(DoorCallback cb);
    bool sendDoorStateEvent(bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
  protected:
    virtual bool onDoorState(bool &state);
    
    bool handleDoorController(SinricProRequest &request);
  
  private:
    EventLimiter event_limiter;
    DoorCallback doorCallback;
};

template <typename T>
DoorController<T>::DoorController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&DoorController<T>::handleDoorController, this, std::placeholders::_1)); 
}

template <typename T>
void DoorController<T>::onDoorState(DoorCallback cb) {
  doorCallback = cb;
}

template <typename T>
bool DoorController<T>::onDoorState(bool &state) {
  T* device = static_cast<T*>(this);
  if (doorCallback) return doorCallback(device->deviceId, state);
  return false;
}

template <typename T>
bool DoorController<T>::sendDoorStateEvent(bool state, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_DOOR_setMode, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  state ? event_value[FSTR_DOOR_mode] = FSTR_DOOR_Close : event_value[FSTR_DOOR_mode] = FSTR_DOOR_Open;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool DoorController<T>::handleDoorController(SinricProRequest &request) {
  bool success = false;
  if (request.action == FSTR_DOOR_setMode) {
    String mode = request.request_value[FSTR_DOOR_mode] | "";
    bool state = mode == FSTR_DOOR_Close;
    success = onDoorState(state);
    request.response_value[FSTR_DOOR_mode] = state ? FSTR_DOOR_Close : FSTR_DOOR_Open;
  }
  return success;
}


} // SINRICPRO_NAMESPACE