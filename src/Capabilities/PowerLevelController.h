#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(POWERLEVEL, setPowerLevel);
FSTR(POWERLEVEL, powerLevel);
FSTR(POWERLEVEL, adjustPowerLevel);
FSTR(POWERLEVEL, powerLevelDelta);

using SetPowerLevelCallback    = std::function<bool(const String &, int &)>;
using AdjustPowerLevelCallback = std::function<bool(const String &, int &)>;

template <typename T>
class PowerLevelController {
  public:
    PowerLevelController();

    void onPowerLevel(SetPowerLevelCallback cb);
    void onAdjustPowerLevel(AdjustPowerLevelCallback cb);

    bool sendPowerLevelEvent(int powerLevel, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    virtual bool onPowerLevel(int &value);
    virtual bool onAdjustPowerLevel(int &valueDelta);
    bool handlePowerLevelController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    SetPowerLevelCallback setPowerLevelCallback;
    AdjustPowerLevelCallback adjustPowerLevelCallback;
};

template <typename T>
PowerLevelController<T>::PowerLevelController() 
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&PowerLevelController<T>::handlePowerLevelController, this, std::placeholders::_1)); 
}

template <typename T>
void PowerLevelController<T>::onPowerLevel(SetPowerLevelCallback cb){
  setPowerLevelCallback = cb;
}

template <typename T>
void PowerLevelController<T>::onAdjustPowerLevel(AdjustPowerLevelCallback cb){
  adjustPowerLevelCallback = cb;
}

template <typename T>
bool PowerLevelController<T>::onPowerLevel(int &value){
  T* device = static_cast<T*>(this);
  if (setPowerLevelCallback) return setPowerLevelCallback(device->deviceId, value);
  return false;  
}

template <typename T>
bool PowerLevelController<T>::onAdjustPowerLevel(int &valueDelta){
  T* device = static_cast<T*>(this);
  if (adjustPowerLevelCallback) return adjustPowerLevelCallback(device->deviceId, valueDelta);
  return false;  
}

template <typename T>
bool PowerLevelController<T>::sendPowerLevelEvent(int powerLevel, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_POWERLEVEL_setPowerLevel, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_POWERLEVEL_powerLevel] = powerLevel;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool PowerLevelController<T>::handlePowerLevelController(SinricProRequest &request) {
  bool success = false;

  if (request.action == FSTR_POWERLEVEL_setPowerLevel) {
    int powerLevel = request.request_value[FSTR_POWERLEVEL_powerLevel];
    success = onPowerLevel(powerLevel);
    request.response_value[FSTR_POWERLEVEL_powerLevel] = powerLevel;
  }

  if (request.action == FSTR_POWERLEVEL_adjustPowerLevel) {
    int powerLevelDelta = request.request_value[FSTR_POWERLEVEL_powerLevelDelta];
    success = onAdjustPowerLevel(powerLevelDelta);
    request.response_value[FSTR_POWERLEVEL_powerLevel] = powerLevelDelta;
  }
  return success;
}

} // SINRICPRO_NAMESPACE