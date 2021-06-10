#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(COLOR, setColor);
FSTR(COLOR, color);
FSTR(COLOR, r);
FSTR(COLOR, g);
FSTR(COLOR, b);

using ColorCallback = std::function<bool(const String &, byte &, byte &, byte &)>;

template <typename T>
class ColorController {
  public:
    ColorController();

    void onColor(ColorCallback cb);
    bool sendColorEvent(byte r, byte g, byte b, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    virtual bool onColor(byte &r, byte &g, byte &b);

    bool handleColorController(SinricProRequest &request);
  private:
    EventLimiter event_limiter;
    ColorCallback colorCallback;
};

template <typename T>
ColorController<T>::ColorController()
: event_limiter(EVENT_LIMIT_STATE) {
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&ColorController<T>::handleColorController, this, std::placeholders::_1)); 
}

template <typename T>
void ColorController<T>::onColor(ColorCallback cb) {
  colorCallback = cb;
}

template <typename T>
bool ColorController<T>::onColor(byte &r, byte &g, byte &b) {
  T* device = static_cast<T*>(this);
  if (colorCallback) return colorCallback(device->deviceId, r, g, b);
  return false;
}

template <typename T>
bool ColorController<T>::sendColorEvent(byte r, byte g, byte b, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_COLOR_setColor, cause.c_str());
  JsonObject event_color = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value].createNestedObject(FSTR_COLOR_color);
  event_color[FSTR_COLOR_r] = r;
  event_color[FSTR_COLOR_g] = g;
  event_color[FSTR_COLOR_b] = b;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ColorController<T>::handleColorController(SinricProRequest &request) {
  bool success = false;

  if (request.action == FSTR_COLOR_setColor) {
    unsigned char r, g, b;
    r = request.request_value[FSTR_COLOR_color][FSTR_COLOR_r];
    g = request.request_value[FSTR_COLOR_color][FSTR_COLOR_g];
    b = request.request_value[FSTR_COLOR_color][FSTR_COLOR_b];
    success = onColor(r, g, b);
    request.response_value.createNestedObject(FSTR_COLOR_color);
    request.response_value[FSTR_COLOR_color][FSTR_COLOR_r] = r;
    request.response_value[FSTR_COLOR_color][FSTR_COLOR_g] = g;
    request.response_value[FSTR_COLOR_color][FSTR_COLOR_b] = b;
  }

  return success;
}

} // SINRICPRO_NAMESPACE