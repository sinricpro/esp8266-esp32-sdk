#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(VOLUME, setVolume);
FSTR(VOLUME, volume);
FSTR(VOLUME, adjustVolume);
FSTR(VOLUME, volumeDefault);

using SetVolumeCallback = std::function<bool(const String &, int &)>;
using AdjustVolumeCallback = std::function<bool(const String &, int &, bool)>;

template <typename T>
class VolumeController {
  public:
    VolumeController();

    void onSetVolume(SetVolumeCallback cb);
    void onAdjustVolume(AdjustVolumeCallback cb);
    bool sendVolumeEvent(int volume, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    virtual bool onSetVolume(int &volume);
    virtual bool onAdjustVolume(int &volumeDelta, bool volumeDefault);
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

template <typename T>
void VolumeController<T>::onSetVolume(SetVolumeCallback cb) {
  volumeCallback = cb;
}

template <typename T>
void VolumeController<T>::onAdjustVolume(AdjustVolumeCallback cb) {
  adjustVolumeCallback = cb;
}

template <typename T>
bool VolumeController<T>::onSetVolume(int &volume) {
  T* device = static_cast<T*>(this);
  if (volumeCallback) return volumeCallback(device->deviceId, volume);
  return false;
}

template <typename T>
bool VolumeController<T>::onAdjustVolume(int &volumeDelta, bool volumeDefault) {
  T* device = static_cast<T*>(this);
  if (adjustVolumeCallback) return adjustVolumeCallback(device->deviceId, volumeDelta, volumeDefault);
  return false;
}

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
  bool success = false;

  if (request.action == FSTR_VOLUME_setVolume) {
    int volume = request.request_value[FSTR_VOLUME_volume];
    success = onSetVolume(volume);
    request.response_value[FSTR_VOLUME_volume] = volume;
    return success;
  }

  if (request.action == FSTR_VOLUME_adjustVolume) {
    int volume = request.request_value[FSTR_VOLUME_volume];
    bool volumeDefault = request.request_value[FSTR_VOLUME_volumeDefault] | false;
    success = onAdjustVolume(volume, volumeDefault);
    request.response_value[FSTR_VOLUME_volume] = volume;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACe