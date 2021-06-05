#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using SetPercentageCallback = std::function<bool(const String &, int &)>;
using AdjustPercentageCallback = std::function<bool(const String &, int &)>;

template <typename T>
class PercentageController {
  public:
    PercentageController();

    void onSetPercentage(SetPercentageCallback cb);
    void onAdjustPercentage(AdjustPercentageCallback cb);
    bool sendSetPercentageEvent(int percentage, String cause = F("PHYSICAL_INTERACTION"));

  protected:
    virtual bool onSetPercentage(int &percentage);
    virtual bool onAdjustPercentage(int &percentageDelta);

    bool handlePercentageController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    SetPercentageCallback    percentageCallback;
    AdjustPercentageCallback adjustPercentageCallback;
};

template <typename T>
PercentageController<T>::PercentageController()
:event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&PercentageController<T>::handlePercentageController, this, std::placeholders::_1)); 
}

template <typename T>
void PercentageController<T>::onSetPercentage(SetPercentageCallback cb) {
  percentageCallback = cb;
}

template <typename T>
void PercentageController<T>::onAdjustPercentage(AdjustPercentageCallback cb) {
  adjustPercentageCallback = cb;
}

template <typename T>
bool PercentageController<T>::onSetPercentage(int &percentage) {
  T* device = static_cast<T*>(this);
  if (percentageCallback) return percentageCallback(device->deviceId, percentage);
  return false;
}

template <typename T>
bool PercentageController<T>::onAdjustPercentage(int &percentageDelta) {
  T* device = static_cast<T*>(this);
  if (adjustPercentageCallback) return adjustPercentageCallback(device->deviceId, percentageDelta);
  return false;
}

template <typename T>
bool PercentageController<T>::sendSetPercentageEvent(int percentage, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setPercentage", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["percentage"] = percentage;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool PercentageController<T>::handlePercentageController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setPercentage") {
    int percentage = request.request_value["percentage"];
    success = onSetPercentage(percentage);
    request.response_value["percentage"] = percentage;
    return success;
  }

  if (request.action == "adjustPercentage") {
    int percentageDelta = request.request_value["percentage"];
    success = onAdjustPercentage(percentageDelta);
    request.response_value["percentage"] = percentageDelta;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE