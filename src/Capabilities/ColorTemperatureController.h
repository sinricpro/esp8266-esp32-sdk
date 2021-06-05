#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using ColorTemperatureCallback = std::function<bool(const String &, int &)>;
using IncreaseColorTemperatureCallback = std::function<bool(const String &, int &)>;
using DecreaseColorTemperatureCallback = std::function<bool(const String &, int &)>;

template <typename T>
class ColorTemperatureController {
  public:
    ColorTemperatureController();

    void onColorTemperature(ColorTemperatureCallback cb);
    void onIncreaseColorTemperature(IncreaseColorTemperatureCallback cb);
    void onDecreaseColorTemperature(DecreaseColorTemperatureCallback cb);

    bool sendColorTemperatureEvent(int colorTemperature, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onColorTemperature(int &colorTemperature);
    virtual bool onIncreaseColorTemperature(int &colorTemperatureDelta);
    virtual bool onDecreaseColorTemperature(int &colorTemperatureDelta);

    bool handleColorTemperatureController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    ColorTemperatureCallback colorTemperatureCallback;
    IncreaseColorTemperatureCallback increaseColorTemperatureCallback;
    DecreaseColorTemperatureCallback decreaseColorTemperatureCallback;
};

template <typename T>
ColorTemperatureController<T>::ColorTemperatureController() 
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&ColorTemperatureController<T>::handleColorTemperatureController, this, std::placeholders::_1)); 
}

template <typename T>
void ColorTemperatureController<T>::onColorTemperature(ColorTemperatureCallback cb) {
  colorTemperatureCallback = cb;
}

template <typename T>
void ColorTemperatureController<T>::onIncreaseColorTemperature(IncreaseColorTemperatureCallback cb) {
  increaseColorTemperatureCallback = cb;
}

template <typename T>
void ColorTemperatureController<T>::onDecreaseColorTemperature(DecreaseColorTemperatureCallback cb) {
  decreaseColorTemperatureCallback = cb;
}

template <typename T>
bool ColorTemperatureController<T>::onColorTemperature(int &colorTemperature) {
  T* device = static_cast<T*>(this);
  if (colorTemperatureCallback) return colorTemperatureCallback(device->deviceId, colorTemperature);
  return false;
}

template <typename T>
bool ColorTemperatureController<T>::onIncreaseColorTemperature(int &colorTemperatureDelta) {
  T* device = static_cast<T*>(this);
  if (increaseColorTemperatureCallback) return increaseColorTemperatureCallback(device->deviceId, colorTemperatureDelta);
  return false;
}

template <typename T>
bool ColorTemperatureController<T>::onDecreaseColorTemperature(int &colorTemperatureDelta) {
  T* device = static_cast<T*>(this);
  if (decreaseColorTemperatureCallback) return decreaseColorTemperatureCallback(device->deviceId, colorTemperatureDelta);
  return false;
}

template <typename T>
bool ColorTemperatureController<T>::sendColorTemperatureEvent(int colorTemperature, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setColorTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["colorTemperature"] = colorTemperature;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ColorTemperatureController<T>::handleColorTemperatureController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setColorTemperature") {
    int colorTemperature = request.request_value["colorTemperature"];
    success = onColorTemperature(colorTemperature);
    request.response_value["colorTemperature"] = colorTemperature;
  }

  if (request.action == "increaseColorTemperature") {
    int colorTemperature = 1;
    success = onIncreaseColorTemperature(colorTemperature);
    request.response_value["colorTemperature"] = colorTemperature;
  }

  if (request.action == "decreaseColorTemperature") {
    int colorTemperature = -1;
    success = onDecreaseColorTemperature(colorTemperature);
    request.response_value["colorTemperature"] = colorTemperature;
  }

  return success;
}

} // SINRICPRO_NAMESPACE