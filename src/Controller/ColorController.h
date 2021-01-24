#ifndef _COLORCONTROLLER_H_
#define _COLORCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

/**
 * @class ColorController
 * @brief supports onColor, sendColorEvent
 **/
class ColorController {
  public:
    ColorController(SinricProDeviceInterface* device);

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
    using ColorCallback = std::function<bool(const String &, byte &, byte &, byte &)>;

    void onColor(ColorCallback cb);
    bool sendColorEvent(byte r, byte g, byte b, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);
  private:
    SinricProDeviceInterface* device;
    ColorCallback colorCallback;
};

ColorController::ColorController(SinricProDeviceInterface* device) : device(device) {}

/**
 * @brief Set callback function for `setColor` request
 * 
 * @param cb Function pointer to a `ColorCallback` function
 * @return void
 * @see ColorCallback
 **/
void ColorController::onColor(ColorCallback cb) {
  colorCallback = cb;
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
bool ColorController::sendColorEvent(byte r, byte g, byte b, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("setColor", cause.c_str());
  JsonObject event_color = eventMessage["payload"]["value"].createNestedObject("color");
  event_color["r"] = r;
  event_color["g"] = g;
  event_color["b"] = b;
  return device->sendEvent(eventMessage);
}

bool ColorController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (colorCallback && actionString == "setColor") {
    unsigned char r, g, b;
    r = request_value["color"]["r"];
    g = request_value["color"]["g"];
    b = request_value["color"]["b"];
    success = colorCallback(device->getDeviceId(), r, g, b);
    response_value.createNestedObject("color");
    response_value["color"]["r"] = r;
    response_value["color"]["g"] = g;
    response_value["color"]["b"] = b;
  }

  return success;
}

#endif