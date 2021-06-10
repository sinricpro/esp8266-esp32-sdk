#pragma once

#include "../SinricProRequest.h"
#include "../SinricProConfig.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(BRIGHTNESS, setBrightness);
FSTR(BRIGHTNESS, brightness);
FSTR(BRIGHTNESS, adjustBrightness);
FSTR(BRIGHTNESS, brightnessDelta);

/**
 * @ingroup Callbacks
 * @param deviceId deviceId
 * @param brightness Value [0..100]
 * @return true     request was handled
 * @return false    request could not be handled
 * #### Example
 * @include callbacks/onBrightness.cpp
 */
using BrightnessCallback       = std::function<bool(const String &, int &)>;

/**
 * @ingroup Callbacks
 * @param deviceId deviceId
 * @param brightnessDelta value between [-100..+100] 
 * @return true     request was handled
 * @return false    request could not be handled
 * #### Example
 * @include callbacks/onAdjustBrightness.cpp
 */
using AdjustBrightnessCallback = std::function<bool(const String &, int &)>;

/**
 * @brief BrightnessController
 * @ingroup Capabilities
 * Supports
 * @ref onBrightness
 * @ref onAdjustBrightness
 * @ref sendBrightnessEvent
 */
template <typename T>
class BrightnessController {
  public:
    BrightnessController();

    void onBrightness(BrightnessCallback cb);
    void onAdjustBrightness(AdjustBrightnessCallback cb);

    bool sendBrightnessEvent(int brightness, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

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

/**
 * @brief Set callback function for Brightness request
 * @param cb Function pointer to a @ref BrightnessCallback
 */
template <typename T>
void BrightnessController<T>::onBrightness(BrightnessCallback cb) {
  brightnessCallback = cb;
}

/**
 * @brief Set callback function for adjust brightness request
 * @param cb Function pointer to an @ref AdjustBrightnessCallback
 */
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

/**
 * @brief Update the brightness on SinricPro Server
 * @param brightness new brightness level
 * @param cause default "PHYSICAL_INTERACTION"
 * @return true event was sent to SinricProServer
 * @return false event could not be send to SinricProServer
 */
template <typename T>
bool BrightnessController<T>::sendBrightnessEvent(int brightness, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_BRIGHTNESS_setBrightness, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_BRIGHTNESS_brightness] = brightness;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool BrightnessController<T>::handleBrightnessController(SinricProRequest &request) {
  bool success = false;

  if (request.action == FSTR_BRIGHTNESS_setBrightness) {
    int brightness = request.request_value[FSTR_BRIGHTNESS_brightness];
    success = onBrightness(brightness);
    request.response_value[FSTR_BRIGHTNESS_brightness] = brightness;
  }

  if (request.action == FSTR_BRIGHTNESS_adjustBrightness) {
    int brightnessDelta = request.request_value[FSTR_BRIGHTNESS_brightnessDelta];
    success = onAdjustBrightness(brightnessDelta);
    request.response_value[FSTR_BRIGHTNESS_brightness] = brightnessDelta;
  }

  return success;
}

// #endif

} // SINRICPRO_NAMESPACE