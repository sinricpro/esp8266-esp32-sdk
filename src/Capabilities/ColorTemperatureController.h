#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(COLORTEMPERATURE, colorTemperature);            // "colorTemperature"
FSTR(COLORTEMPERATURE, setColorTemperature);         // "setColorTemperature"
FSTR(COLORTEMPERATURE, increaseColorTemperature);    // "increaseColorTemperature"
FSTR(COLORTEMPERATURE, decreaseColorTemperature);    // "decreaseColorTemperature"

/**
 * @brief Callback definition for onColorTemperature function
 * 
 * Gets called when device receive a `setColorTemperature` request \n
 * @param[in]   deviceId          String which contains the ID of device
 * @param[in]   colorTemperature  Integer value with color temperature the device should set to \n `2200` = warm white \n `2700` = soft white \n `4000` = white \n `5500` = daylight white \n `7000` = cool white
 * @param[out]  colorTemperature  Integer value with color temperature the device is set to \n `2200` = warm white \n `2700` = soft white \n `4000` = white \n `5500` = daylight white \n `7000` = cool white
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * @section ColorTemperatureCallback Example-Code
 * @snippet callbacks.cpp onColorTemperature
 **/
using ColorTemperatureCallback = std::function<bool(const String &, int &)>;

/**
 * @brief Callback definition for onIncreaseColorTemperature function
 * 
 * Gets called when device receive a `increaseColorTemperature` request \n
 * @param[in]   deviceId          String which contains the ID of device
 * @param[in]   colorTemperature  Integer value `1` = Device should increase color temperature
 * @param[out]  colorTemperature  Integer value return the new color temperarature \n `2200` = warm white \n `2700` = soft white \n `4000` = white \n `5500` = daylight white \n `7000` = cool white
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * @section IncreaseColorTemperatureCallback Example-Code
 * @snippet callbacks.cpp onIncreaseColorTemperature
 **/
using IncreaseColorTemperatureCallback = std::function<bool(const String &, int &)>;

/**
 * @brief Callback definition for onDecreaseColorTemperature function
 * 
 * Gets called when device receive a `decreaseColorTemperature` request \n
 * @param[in]   deviceId          String which contains the ID of device
 * @param[in]   colorTemperature  Integer value `-1` = Device should decrease color temperature
 * @param[out]  colorTemperature  Integer value return the new color temperarature \n `2200` = warm white \n `2700` = soft white \n `4000` = white \n `5500` = daylight white \n `7000` = cool white
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * @section DecreaseColorTemperatureCallback Example-Code
 * @snippet callbacks.cpp onDecreaseColorTemperature
 **/
using DecreaseColorTemperatureCallback = std::function<bool(const String &, int &)>;


/**
 * @brief ColorTemperatureController
 * @ingroup Capabilities
 **/
template <typename T>
class ColorTemperatureController {
  public:
    ColorTemperatureController();

    void onColorTemperature(ColorTemperatureCallback cb);
    void onIncreaseColorTemperature(IncreaseColorTemperatureCallback cb);
    void onDecreaseColorTemperature(DecreaseColorTemperatureCallback cb);

    bool sendColorTemperatureEvent(int colorTemperature, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handleColorTemperatureController(SinricProRequest &request);

  private: 
    EventLimiter event_limiter;
//    SinricProDeviceInterface *device;
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

/**
 * @brief Set callback function for `setColorTemperature` request
 * 
 * @param cb Function pointer to a `ColorTemperatureCallback` function
 * @return void
 * @see ColorTemperatureCallback
 **/
template <typename T>
void ColorTemperatureController<T>::onColorTemperature(ColorTemperatureCallback cb) {
  colorTemperatureCallback = cb;
}

/**
 * @brief Set callback function for `increaseColorTemperature` request
 * 
 * @param cb Function pointer to a `IncreaseColorTemperatureCallback` function
 * @return void
 * @see IncreaseColorTemperatureCallback
 **/
template <typename T>
void ColorTemperatureController<T>::onIncreaseColorTemperature(IncreaseColorTemperatureCallback cb) {
  increaseColorTemperatureCallback = cb;
}

/**
 * @brief Set callback function for `decreaseColorTemperature` request
 * 
 * @param cb Function pointer to a `DecreaseColorTemperatureCallback` function
 * @return void
 * @see DecreaseColorTemperatureCallback
 **/
template <typename T>
void ColorTemperatureController<T>::onDecreaseColorTemperature(DecreaseColorTemperatureCallback cb) {
  decreaseColorTemperatureCallback = cb;
}

/**
 * @brief Send `setColorTemperature` event to SinricPro Server indicating actual color temperature
 * 
 * @param colorTemperature Integer with new color temperature the device is set to \n `2200` = warm white \n `2700` = soft white \n `4000` = white \n `5500` = daylight white \n `7000` = cool white
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ColorTemperatureController<T>::sendColorTemperatureEvent(int colorTemperature, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_COLORTEMPERATURE_setColorTemperature, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_COLORTEMPERATURE_colorTemperature] = colorTemperature;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool ColorTemperatureController<T>::handleColorTemperatureController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (colorTemperatureCallback && request.action == FSTR_COLORTEMPERATURE_setColorTemperature) {
    int colorTemperature = request.request_value[FSTR_COLORTEMPERATURE_colorTemperature];
    success = colorTemperatureCallback(device->deviceId, colorTemperature);
    request.response_value[FSTR_COLORTEMPERATURE_colorTemperature] = colorTemperature;
  }

  if (increaseColorTemperatureCallback && request.action == FSTR_COLORTEMPERATURE_increaseColorTemperature) {
    int colorTemperature = 1;
    success = increaseColorTemperatureCallback(device->deviceId, colorTemperature);
    request.response_value[FSTR_COLORTEMPERATURE_colorTemperature] = colorTemperature;
  }

  if (decreaseColorTemperatureCallback && request.action == FSTR_COLORTEMPERATURE_decreaseColorTemperature) {
    int colorTemperature = -1;
    success = decreaseColorTemperatureCallback(device->deviceId, colorTemperature);
    request.response_value[FSTR_COLORTEMPERATURE_colorTemperature] = colorTemperature;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using ColorTemperatureController = SINRICPRO_NAMESPACE::ColorTemperatureController<T>;