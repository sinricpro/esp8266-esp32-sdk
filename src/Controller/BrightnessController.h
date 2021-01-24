#ifndef _BRIGHTNESSCONTROLLER_H_
#define _BRIGHTNESSCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

/**
 * @class: BrightnessController
 * @brief: support for onBrightness, adjustBrightness, sendBrightnessEvent
 **/
class BrightnessController {
  public:
    BrightnessController(SinricProDeviceInterface *device);

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
    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
    SinricProDeviceInterface* device;
    BrightnessCallback brightnessCallback;
    AdjustBrightnessCallback adjustBrightnessCallback;
};

BrightnessController::BrightnessController(SinricProDeviceInterface* device) : device(device) {}

/**
 * @brief Set callback function for `setBrightness` request
 * 
 * @param cb Function pointer to a `BrightnessCallback` function
 * @return void
 * @see BrightnessCallback
 **/
void BrightnessController::onBrightness(BrightnessCallback cb) {
  brightnessCallback = cb;
}

/**
 * @brief Set callback function for `adjustBrightness` request
 * 
 * @param cb Function pointer to a `AdjustBrightnessCallback` function
 * @return void
 * @see AdjustBrightnessCallback
 **/
void BrightnessController::onAdjustBrightness(AdjustBrightnessCallback cb) {
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
bool BrightnessController::sendBrightnessEvent(int brightness, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("setBrightness", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["brightness"] = brightness;
  return device->sendEvent(eventMessage);
}

bool BrightnessController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (brightnessCallback && actionString == "setBrightness") {
    int brightness = request_value["brightness"];
    success = brightnessCallback(device->getDeviceId(), brightness);
    response_value["brightness"] = brightness;
  }

  if (adjustBrightnessCallback && actionString == "adjustBrightness") {
    int brightnessDelta = request_value["brightnessDelta"];
    success = adjustBrightnessCallback(device->getDeviceId(), brightnessDelta);
    response_value["brightness"] = brightnessDelta;
  }

  return success;
}

#endif