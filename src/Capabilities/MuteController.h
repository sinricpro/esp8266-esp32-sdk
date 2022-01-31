#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(MUTE, mute);     // "mute"
FSTR(MUTE, setMute);  // "setMute"

/**
 * @brief Callback definition for onMute function
 * 
 * Gets called when device receive a `setMute` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   mute        `true` mute device \n `false` unmute device
 * @param[out]  mute        `true` device is muted \n `false` device is unmuted
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section MuteCallback Example-Code
 * @snippet callbacks.cpp onMute
 **/
using MuteCallback = std::function<bool(const String &, bool &)>;


/**
 * @brief MuteController
 * @ingroup Capabilities
 **/
template <typename T>
class MuteController {
  public:
    MuteController();

    void onMute(MuteCallback cb);
    bool sendMuteEvent(bool mute, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
  protected:
    bool handleMuteController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    MuteCallback muteCallback;
};

template <typename T>
MuteController<T>::MuteController()
:event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&MuteController<T>::handleMuteController, this, std::placeholders::_1)); 
}

/**
 * @brief Set callback function for `setMute` request
 * 
 * @param cb Function pointer to a `MuteCallback` function
 * @return void
 * @see MuteCallback
 **/
template <typename T>
void MuteController<T>::onMute(MuteCallback cb) { muteCallback = cb; }

/**
 * @brief Send `setMute` event to SinricPro Server indicating actual mute state
 * 
 * @param mute    `true` = device is muted on \n `false` = device is unmuted
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool MuteController<T>::sendMuteEvent(bool mute, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_MUTE_setMute, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_MUTE_mute] = mute;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool MuteController<T>::handleMuteController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (muteCallback && request.action == FSTR_MUTE_setMute) {
    bool mute = request.request_value[FSTR_MUTE_mute];
    success = muteCallback(device->deviceId, mute);
    request.response_value[FSTR_MUTE_mute] = mute;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using MuteController = SINRICPRO_NAMESPACE::MuteController<T>;