#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(OPEN_CLOSE, openPercent);      // "openPercent"
FSTR(OPEN_CLOSE, openDirection);    // "openDirection"
FSTR(OPEN_CLOSE, setOpenClose);     // "setOpenClose" 
FSTR(OPEN_CLOSE, adjustOpenClose);  // "adjustOpenClose" 

using OpenCloseCallback = std::function<bool(const String &, const String &, int &)>;

using AdjustOpenCloseCallback = std::function<bool(const String &, const String &, int &)>;

template <typename T>
class OpenCloseController {
  public:
    OpenCloseController();

    void onOpenClose(OpenCloseCallback cb);

    void onAdjustOpenClose(AdjustOpenCloseCallback cb);

    bool sendOpenCloseEvent(int openPercent, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

    bool sendOpenCloseEvent(String openDirection, int openPercent, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handleOpenCloseController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    OpenCloseCallback openCloseCallbackCallback;
    AdjustOpenCloseCallback adjustOpenCloseCallback; 
};

template <typename T>
OpenCloseController<T>::OpenCloseController()
:event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&OpenCloseController<T>::handleOpenCloseController, this, std::placeholders::_1)); 
}

template <typename T>
void OpenCloseController<T>::onOpenClose(OpenCloseCallback cb) { openCloseCallbackCallback = cb; }

template <typename T>
void OpenCloseController<T>::onAdjustOpenClose(AdjustOpenCloseCallback cb) { adjustOpenCloseCallback = cb; }

template <typename T>
bool OpenCloseController<T>::sendOpenCloseEvent(String openDirection, int openPercent, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  JsonDocument eventMessage = device->prepareEvent(FSTR_OPEN_CLOSE_setOpenClose, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_OPEN_CLOSE_openDirection] = openDirection;
  event_value[FSTR_OPEN_CLOSE_openPercent] = openPercent;

  return device->sendEvent(eventMessage);
}

template <typename T>
bool OpenCloseController<T>::sendOpenCloseEvent(int openPercent, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  JsonDocument eventMessage = device->prepareEvent(FSTR_OPEN_CLOSE_setOpenClose, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_OPEN_CLOSE_openPercent] = openPercent;

  return device->sendEvent(eventMessage);
}

template <typename T>
bool OpenCloseController<T>::handleOpenCloseController(SinricProRequest &request) {
  if (request.action != FSTR_OPEN_CLOSE_setOpenClose) {
    return false;
  }

  T* device = static_cast<T*>(this);

  bool success = false;

  if (openCloseCallbackCallback && request.action == FSTR_OPEN_CLOSE_setOpenClose) {
    String openDirection = request.request_value[FSTR_OPEN_CLOSE_openDirection];
    int openPercent = request.request_value[FSTR_OPEN_CLOSE_openPercent];

    success = openCloseCallbackCallback(device->deviceId, openDirection, openPercent);

    request.response_value[FSTR_OPEN_CLOSE_openDirection] = openDirection;
    request.response_value[FSTR_OPEN_CLOSE_openPercent] = openPercent; 
    
    return success;
  }

  if (adjustOpenCloseCallback && request.action == FSTR_OPEN_CLOSE_adjustOpenClose) {
     // TOOD:
  }

  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using OpenCloseController = SINRICPRO_NAMESPACE::OpenCloseController<T>;