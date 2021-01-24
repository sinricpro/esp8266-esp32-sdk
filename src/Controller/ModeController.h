#ifndef _MODECONTROLLER_H_
#define _MODECONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class ModeController {
  public:
    ModeController(SinricProDeviceInterface *device);

    /**
     * @brief Callback definition for onSetMode function
     * 
     * Gets called when device receive a `setBands` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   mode        String device mode should set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
     * @param[out]  mode        String devices mode is set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section ModeCallback Example-Code
     * @snippet callbacks.cpp onSetMode
     **/
    using ModeCallback = std::function<bool(const String &, String &)>;

    void onSetMode(ModeCallback cb);

    bool sendModeEvent(String mode, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
    SinricProDeviceInterface *device;
    ModeCallback setModeCallback;
};

ModeController::ModeController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Set callback function for `setMode` request
 * 
 * @param cb Function pointer to a `ModeCallback` function
 * @return void
 * @see ModeCallback
 **/
void ModeController::onSetMode(ModeCallback cb) { setModeCallback = cb; }

/**
 * @brief Send `setMode` event to SinricPro Server indicating the mode has changed
 * 
 * @param mode    String with actual mode device is set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool ModeController::sendModeEvent(String mode, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("setMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mode"] = mode;
  return device->sendEvent(eventMessage);
}

bool ModeController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (setModeCallback && actionString == "setMode") {
    String mode = request_value["mode"] | "";
    success = setModeCallback(device->getDeviceId(), mode);
    response_value["mode"] = mode;
    return success;
  }

  return success;
}

#endif