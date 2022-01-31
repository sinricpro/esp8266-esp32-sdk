#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(DOOR, setMode);   // "setMode"
FSTR(DOOR, mode);      // "mode"
FSTR(DOOR, Close);     // "Close"
FSTR(DOOR, Open);      // "Open"


/**
 * @brief Callback definition for onDoorState function
 * 
 * Gets called when device receive a `open` or `close` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   doorState   `false` = open, device is requested to open the garage door \n`true` = close, device is requested to close the garage door
 * @param[out]  doorState   bool with actual state `false` = open, `true` = closed
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * @section DoorStateCallback Example-Code
 * @snippet callbacks.cpp onDoorState
 **/
using DoorCallback = std::function<bool(const String &, bool &)>;

/**
 * @brief DoorController - only used for GarageDoor device and cannot used as capability for a custom device!
 * 
 **/
template <typename T>
class DoorController {
  public:
    DoorController();

    void onDoorState(DoorCallback cb);
    bool sendDoorStateEvent(bool state, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
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

/**
 * @brief Set callback function for `onDoorState` request
 * 
 * @param cb Function pointer to a `onDoorState` function
 * @return void
 * @see DoorStateCallback
 **/
template <typename T>
void DoorController<T>::onDoorState(DoorCallback cb) { doorCallback = cb; }

/**
 * @brief Send `DoorState` event to update actual door state on SinricPro Server
 * 
 * @param state   bool `true` = close \n `false` = open
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
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
  T* device = static_cast<T*>(this);

  bool success = false;
  if (request.action == FSTR_DOOR_setMode && doorCallback) {
    String mode = request.request_value[FSTR_DOOR_mode] | "";
    bool state = mode == FSTR_DOOR_Close;
    success = doorCallback(device->deviceId, state);
    request.response_value[FSTR_DOOR_mode] = state ? FSTR_DOOR_Close : FSTR_DOOR_Open;
  }
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using DoorController = SINRICPRO_NAMESPACE::DoorController<T>;