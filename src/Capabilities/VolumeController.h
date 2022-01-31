#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(VOLUME, setVolume);         // "setVolume"
FSTR(VOLUME, volume);            // "volume"
FSTR(VOLUME, adjustVolume);      // "adjustVolume"
FSTR(VOLUME, volumeDefault);     // "volumeDefault"

/**
 * @brief Callback definition for onSetVolume function
 * 
 * Gets called when device receive a `setVolume` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   volume      Integer with volume device should set to
 * @param[out]  volume      Integer with volume device has been set to
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section SetVolumeCallback Example-Code
 * @snippet callbacks.cpp onSetVolume
 **/
using SetVolumeCallback = std::function<bool(const String &, int &)>;

/**
 * @brief Callback definition for onAdjustVolume function
 * 
 * Gets called when device receive a `adjustVolume` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   volumeDelta Integer with relative volume the device should change about (-100..100)
 * @param[out]  volumeDelta Integer with absolute volume device has been set to
 * @param[in]   volumeDefault Bool `false` if the user specified the amount by which to change the volume; otherwise `true`
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section AdjustVolumeCallback Example-Code
 * @snippet callbacks.cpp onAdjustVolume
 **/
using AdjustVolumeCallback = std::function<bool(const String &, int &, bool)>;


/**
 * @brief VolumeController
 * @ingroup Capabilities
 **/
template <typename T>
class VolumeController {
  public:
    VolumeController();

    void onSetVolume(SetVolumeCallback cb);
    void onAdjustVolume(AdjustVolumeCallback cb);

    bool sendVolumeEvent(int volume, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handleVolumeController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    SetVolumeCallback volumeCallback;
    AdjustVolumeCallback adjustVolumeCallback;
};

template <typename T>
VolumeController<T>::VolumeController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&VolumeController<T>::handleVolumeController, this, std::placeholders::_1)); 
}

/**
 * @brief Set callback function for `setVolume` request
 * 
 * @param cb Function pointer to a `SetVolumeCallback` function
 * @return void
 * @see SetVolumeCallback
 **/
template <typename T>
void VolumeController<T>::onSetVolume(SetVolumeCallback cb) { volumeCallback = cb; }

/**
 * @brief Set callback function for `adjustVolume` request
 * 
 * @param cb Function pointer to a `AdjustVolumeCallback` function
 * @return void
 * @see AdjustVolumeCallback
 **/
template <typename T>
void VolumeController<T>::onAdjustVolume(AdjustVolumeCallback cb) { adjustVolumeCallback = cb; }

/**
 * @brief Send `setVolume` event to SinricPro Server indicating actual volume has changed
 * 
 * @param   volume        Integer reporting the volume that the device have been set to
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool VolumeController<T>::sendVolumeEvent(int volume, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_VOLUME_setVolume, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_VOLUME_volume] = volume;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool VolumeController<T>::handleVolumeController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (volumeCallback && request.action == FSTR_VOLUME_setVolume) {
    int volume = request.request_value[FSTR_VOLUME_volume];
    success = volumeCallback(device->deviceId, volume);
    request.response_value[FSTR_VOLUME_volume] = volume;
    return success;
  }

  if (adjustVolumeCallback && request.action == FSTR_VOLUME_adjustVolume) {
    int volume = request.request_value[FSTR_VOLUME_volume];
    bool volumeDefault = request.request_value[FSTR_VOLUME_volumeDefault] | false;
    success = adjustVolumeCallback(device->deviceId, volume, volumeDefault);
    request.response_value[FSTR_VOLUME_volume] = volume;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using VolumeController = SINRICPRO_NAMESPACE::VolumeController<T>;