#pragma once

#include "../SinricProRequest.h"
#include "../SinricProConfig.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using BrightnessCallback       = std::function<bool(const String &, int &)>;
using AdjustBrightnessCallback = std::function<bool(const String &, int &)>;

template <typename T>
class BrightnessController {
  public:
    BrightnessController();

    void onBrightness(BrightnessCallback cb);
    void onAdjustBrightness(AdjustBrightnessCallback cb);

    bool sendBrightnessEvent(int brightness, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onBrightness(int &brightness);
    virtual bool onAdjustBrightness(int &brightnessDelta);
    bool handleBrightnessController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    BrightnessCallback       brightnessCallback;
    AdjustBrightnessCallback adjustBrightnessCallback;
};

template <typename T>
BrightnessController<T>::BrightnessController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&BrightnessController<T>::handleBrightnessController, this, std::placeholders::_1)); 
}

template <typename T>
void BrightnessController<T>::onBrightness(BrightnessCallback cb) {
  brightnessCallback = cb;
}

template <typename T>
void BrightnessController<T>::onAdjustBrightness(AdjustBrightnessCallback cb) {
  adjustBrightnessCallback = cb;
}

template <typename T>
bool BrightnessController<T>::onBrightness(int &brightness) {
  T* device = static_cast<T*>(this);
  if (brightnessCallback) return brightnessCallback(device->deviceId, brightness);
  return false;
}

template <typename T>
bool BrightnessController<T>::onAdjustBrightness(int &brightnessDelta) {
  T* device = static_cast<T*>(this);
  if (adjustBrightnessCallback) return adjustBrightnessCallback(device->deviceId, brightnessDelta);
  return false;
}


template <typename T>
bool BrightnessController<T>::sendBrightnessEvent(int brightness, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setBrightness", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["brightness"] = brightness;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool BrightnessController<T>::handleBrightnessController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setBrightness") {
    int brightness = request.request_value["brightness"];
    success = onBrightness(brightness);
    request.response_value["brightness"] = brightness;
  }

  if (request.action == "adjustBrightness") {
    int brightnessDelta = request.request_value["brightnessDelta"];
    success = onAdjustBrightness(brightnessDelta);
    request.response_value["brightness"] = brightnessDelta;
  }

  return success;
}

// #endif

} // SINRICPRO_NAMESPACE