#ifndef _BRIGHTNESSCONTROLLER_H_
#define _BRIGHTNESSCONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief BrightnessController
 * @ingroup Capabilities
 **/
template <typename T>
class BrightnessController {
  public:
    BrightnessController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&BrightnessController<T>::handleBrightnessController, this, std::placeholders::_1)); }
    /**
     * @brief Callback definition for onBrightness function
     * 
     * Gets called when device receive a `setBrightness` request \n
     * @param[in]   deviceId      String which contains the ID of device
     * @param[in]   brightness    Absolute integer value the device should set its brightness to
     * @param[out]  brightness    Absolute integer value with new brightness the device is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section BrightnessCallback Example-Code
     * @snippet callbacks.cpp onBrightness
     **/
    using BrightnessCallback = std::function<bool(const String &, int &)>;

    /**
     * @brief Callback definition for onAdjustBrightness function
     * 
     * Gets called when device receive a `adjustBrightness` request \n
     * @param[in]   deviceId      String which contains the ID of device
     * @param[in]   brightness    Relative integer value the device should change the brightness about
     * @param[out]  brightness    Absolute integer value with new brightness the device is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section AdjustBrightnessCallback Example-Code
     * @snippet callbacks.cpp onAdjustBrightness
     **/
    using AdjustBrightnessCallback = std::function<bool(const String &, int &)>;

    void onBrightness(BrightnessCallback cb);
    void onAdjustBrightness(AdjustBrightnessCallback cb);

    bool sendBrightnessEvent(int brightness, String cause = "PHYSICAL_INTERACTION");
  protected:
    bool handleBrightnessController(SinricProRequest &request);

  private:
    BrightnessCallback brightnessCallback;
    AdjustBrightnessCallback adjustBrightnessCallback;
};


/**
 * @brief Set callback function for `setBrightness` request
 * 
 * @param cb Function pointer to a `BrightnessCallback` function
 * @return void
 * @see BrightnessCallback
 **/
template <typename T>
void BrightnessController<T>::onBrightness(BrightnessCallback cb) {
  brightnessCallback = cb;
}

/**
 * @brief Set callback function for `adjustBrightness` request
 * 
 * @param cb Function pointer to a `AdjustBrightnessCallback` function
 * @return void
 * @see AdjustBrightnessCallback
 **/
template <typename T>
void BrightnessController<T>::onAdjustBrightness(AdjustBrightnessCallback cb) {
  adjustBrightnessCallback = cb;
}

/**
 * @brief Send `setBrightness` event to SinricPro Server indicating actual brightness
 * 
 * @param brightness    Integer value with actual brightness the device is set to
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool BrightnessController<T>::sendBrightnessEvent(int brightness, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setBrightness", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["brightness"] = brightness;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool BrightnessController<T>::handleBrightnessController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);
  bool success = false;

  if (brightnessCallback && request.action == "setBrightness") {
    int brightness = request.request_value["brightness"];
    success = brightnessCallback(device.deviceId, brightness);
    request.response_value["brightness"] = brightness;
  }

  if (adjustBrightnessCallback && request.action == "adjustBrightness") {
    int brightnessDelta = request.request_value["brightnessDelta"];
    success = adjustBrightnessCallback(device.deviceId, brightnessDelta);
    request.response_value["brightness"] = brightnessDelta;
  }

  return success;
}

#endif