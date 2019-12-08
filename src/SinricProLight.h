/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICLIGHT_H_
#define _SINRICLIGHT_H_

#include "SinricProDevice.h"

/**
 * @class SinricProLight
 * @brief Device to control a light 
 * 
 * Supporting 
 * * On / Off
 * * Brightness (0..100)
 * * Color (RGB)
 * * Color temperature
 **/
class SinricProLight :  public SinricProDevice {
  public:
    SinricProLight(const char* deviceId, unsigned long eventWaitTime=100);
    // callback

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
    typedef std::function<bool(const String&, int&)> BrightnessCallback;

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
    typedef std::function<bool(const String&, int&)> AdjustBrightnessCallback;

    /**
     * @brief Callback definition for onColor function
     * 
     * Gets called when device receive a `setColor` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   r           Byte value for red
     * @param[in]   g           Byte value for green
     * @param[in]   b           Byte value for blue
     * @param[out]  r           Byte value for red
     * @param[out]  g           Byte value for green
     * @param[out]  b           Byte value for blue
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section ColorCallback Example-Code
     * @snippet callbacks.cpp onColor
     **/
    typedef std::function<bool(const String&, byte&, byte&, byte&)> ColorCallback;

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
    typedef std::function<bool(const String&, int&)> ColorTemperatureCallback;

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
    typedef std::function<bool(const String&, int&)> IncreaseColorTemperatureCallback;
    
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
    typedef std::function<bool(const String&, int&)> DecreaseColorTemperatureCallback;
  
    void onBrightness(BrightnessCallback cb);
    void onAdjustBrightness(AdjustBrightnessCallback cb);
    void onColor(ColorCallback cb);
    void onColorTemperature(ColorTemperatureCallback cb);
    void onIncreaseColorTemperature(IncreaseColorTemperatureCallback cb);
    void onDecreaseColorTemperature(DecreaseColorTemperatureCallback cb);

    // event
    bool sendBrightnessEvent(int brightness, String cause = "PHYSICAL_INTERACTION");
    bool sendColorEvent(byte r, byte g, byte b, String cause = "PHYSICAL_INTERACTION");
    bool sendColorTemperatureEvent(int colorTemperature, String cause = "PHYSICAL_INTERACTION");

    // handle
    bool handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) override;
  private:
    BrightnessCallback brightnessCallback;
    AdjustBrightnessCallback adjustBrightnessCallback;
    ColorCallback colorCallback;
    ColorTemperatureCallback colorTemperatureCallback;
    IncreaseColorTemperatureCallback increaseColorTemperatureCallback;
    DecreaseColorTemperatureCallback decreaseColorTemperatureCallback;
};

SinricProLight::SinricProLight(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime),
  brightnessCallback(nullptr),
  adjustBrightnessCallback(nullptr),
  colorCallback(nullptr),
  colorTemperatureCallback(nullptr),
  increaseColorTemperatureCallback(nullptr),
  decreaseColorTemperatureCallback(nullptr) {}

bool SinricProLight::handleRequest(const char* deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) {
  if (strcmp(deviceId, this->deviceId) != 0) return false;
  if (SinricProDevice::handleRequest(deviceId, action, request_value, response_value)) return true;

  bool success = false;
  String actionString = String(action);

  if (brightnessCallback && actionString == "setBrightness") {
    int brightness = request_value["brightness"];
    success = brightnessCallback(String(deviceId), brightness);
    response_value["brightness"] = brightness;
  }

  if (adjustBrightnessCallback && actionString == "adjustBrightness") {
    int brightnessDelta = request_value["brightnessDelta"];
    success = adjustBrightnessCallback(String(deviceId), brightnessDelta);
    response_value["brightness"] = brightnessDelta;
  }

  if (colorCallback && actionString == "setColor") {
      unsigned char r, g, b;
      r = request_value["color"]["r"];
      g = request_value["color"]["g"];
      b = request_value["color"]["b"];
      success = colorCallback(String(deviceId), r, g, b);
      response_value.createNestedObject("color");
      response_value["color"]["r"] = r;
      response_value["color"]["g"] = g;
      response_value["color"]["b"] = b;
  }

  if (colorTemperatureCallback && actionString == "setColorTemperature") {
    int colorTemperature = request_value["colorTemperature"];
    success = colorTemperatureCallback(String(deviceId), colorTemperature);
    response_value["colorTemperature"] = colorTemperature;
  }

  if (increaseColorTemperatureCallback && actionString == "increaseColorTemperature") {
    int colorTemperature = 1;
    success = increaseColorTemperatureCallback(String(deviceId), colorTemperature);
    response_value["colorTemperature"] = colorTemperature;
  }

  if (decreaseColorTemperatureCallback && actionString == "decreaseColorTemperature") {
    int colorTemperature = -1;
    success = decreaseColorTemperatureCallback(String(deviceId), colorTemperature);
    response_value["colorTemperature"] = colorTemperature;
  }

  return success;
}

/**
 * @brief Set callback function for `setBrightness` request
 * 
 * @param cb Function pointer to a `BrightnessCallback` function
 * @return void
 * @see BrightnessCallback
 **/
void SinricProLight::onBrightness(BrightnessCallback cb) { 
  brightnessCallback = cb;
}

/**
 * @brief Set callback function for `adjustBrightness` request
 * 
 * @param cb Function pointer to a `AdjustBrightnessCallback` function
 * @return void
 * @see AdjustBrightnessCallback
 **/
void SinricProLight::onAdjustBrightness(AdjustBrightnessCallback cb) { 
  adjustBrightnessCallback = cb; 
}

/**
 * @brief Set callback function for `setColor` request
 * 
 * @param cb Function pointer to a `ColorCallback` function
 * @return void
 * @see ColorCallback
 **/
void SinricProLight::onColor(ColorCallback cb) { 
  colorCallback = cb; 
}

/**
 * @brief Set callback function for `setColorTemperature` request
 * 
 * @param cb Function pointer to a `ColorTemperatureCallback` function
 * @return void
 * @see ColorTemperatureCallback
 **/
void SinricProLight::onColorTemperature(ColorTemperatureCallback cb) { 
  colorTemperatureCallback = cb; 
}

/**
 * @brief Set callback function for `increaseColorTemperature` request
 * 
 * @param cb Function pointer to a `IncreaseColorTemperatureCallback` function
 * @return void
 * @see IncreaseColorTemperatureCallback
 **/
void SinricProLight::onIncreaseColorTemperature(IncreaseColorTemperatureCallback cb) { 
  increaseColorTemperatureCallback = cb; 
}

/**
 * @brief Set callback function for `decreaseColorTemperature` request
 * 
 * @param cb Function pointer to a `DecreaseColorTemperatureCallback` function
 * @return void
 * @see DecreaseColorTemperatureCallback
 **/
void SinricProLight::onDecreaseColorTemperature(DecreaseColorTemperatureCallback cb) {
  decreaseColorTemperatureCallback = cb;
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
bool SinricProLight::sendBrightnessEvent(int brightness, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setBrightness", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["brightness"] = brightness;
  return sendEvent(eventMessage);
}

/**
 * @brief Send `setColor` event to SinricPro Server indicating actual color
 * 
 * @param r       Byte value for red
 * @param g       Byte value for green
 * @param b       Byte value for blue
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool SinricProLight::sendColorEvent(byte r, byte g, byte b, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setColor", cause.c_str());
  JsonObject event_color = eventMessage["payload"]["value"].createNestedObject("color");
  event_color["r"] = r;
  event_color["g"] = g;
  event_color["b"] = b;
  return sendEvent(eventMessage);
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
bool SinricProLight::sendColorTemperatureEvent(int colorTemperature, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "setColorTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["colorTemperature"] = colorTemperature;
  return sendEvent(eventMessage);
}


#endif

