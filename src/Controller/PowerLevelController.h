#ifndef _POWERLEVELCONTROLLER_H_
#define _POWERLEVELCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

/**
 * @class PowerLevelController
 * @brief support for setPowerLevel, adjustPowerLevel and sendPowerLevel
 **/ 
class PowerLevelController {
  public:
    PowerLevelController(SinricProDeviceInterface* device);
    /**
     * @brief Definition for setPowerLevel callback
     * 
     * Gets called when device receive a `setPowerLevel` reuqest \n
     * @param[in]     deviceId        String which contains the ID of device
     * @param[in]     powerLevel      integer `0..100` new powerlevel the device should be set to
     * @param[out]    powerLevel      integer `0..100` report the powerlevel that the device have been set to
     * @return        the success of the request
     * @retval        true            request handled properly
     * @retval        false           request was not handled properly because of some error
     * 
     * @section SetPowerLevelCallback Example-Code
     * @snippet callbacks.cpp onPowerLevel
     **/
    using SetPowerLevelCallback = std::function<bool(const String &, int &)>;

    /**
     * @brief Definition for onAdjustPowerLevel callback
     * 
     * Gets called when device receive a `adjustPowerLevel` reuqest \n
     * @param[in]     deviceId      String which contains the ID of device
     * @param[in]     powerLevel    integer `-100..100` delta value which power level have to be changed
     * @param[out]    powerLevel    integer `0..100` report the absolute powerlevel that the device have been set to
     * @return        the success of the request
     * @retval true   request handled properly
     * @retval false  request can`t be handled properly because of some error
     * 
     * @section AdjustPowerLevelCallback Example-Code
     * @snippet callbacks.cpp onAdjustPowerLevel
     **/
    using AdjustPowerLevelCallback = std::function<bool(const String &, int &)>;

    void onPowerLevel(SetPowerLevelCallback cb);
    void onAdjustPowerLevel(AdjustPowerLevelCallback cb);
    bool sendPowerLevelEvent(int powerLevel, String cause = "PHYSICAL_INTERACTION");

    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
    SinricProDeviceInterface* device;
    SetPowerLevelCallback setPowerLevelCallback;
    AdjustPowerLevelCallback adjustPowerLevelCallback;
};

PowerLevelController::PowerLevelController(SinricProDeviceInterface* device) : device(device) {}

/**
 * @brief Set callback function for setPowerLevel request
 * 
 * @param     cb    Function pointer to a SetPowerLevelCallback function
 * @see       SetPowerLevelCallback
 **/
void PowerLevelController::onPowerLevel(SetPowerLevelCallback cb) {
  setPowerLevelCallback = cb;
}

/**
 * @brief Set callback function for adjustPowerLevel request
 * 
 * @param     cb    Function pointer to a AdjustPowerLevelCallback function
 * @see       AdjustPowerLevelCallback
 **/
void PowerLevelController::onAdjustPowerLevel(AdjustPowerLevelCallback cb) {
  adjustPowerLevelCallback = cb;
}

/**
 * @brief Send `setPowerLevel` event to SinricPro Server indicating actual power level
 * 
 * @param   powerLevel    integer `0..100` report the powerlevel that the device have been set to
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool PowerLevelController::sendPowerLevelEvent(int powerLevel, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent(device->getDeviceId(), "setPowerLevel", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["powerLevel"] = powerLevel;
  return device->sendEvent(eventMessage);
}

bool PowerLevelController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (setPowerLevelCallback && actionString == "setPowerLevel") {
    int powerLevel = request_value["powerLevel"];
    success = setPowerLevelCallback(device->getDeviceId(), powerLevel);
    response_value["powerLevel"] = powerLevel;
  }

  if (adjustPowerLevelCallback && actionString == "adjustPowerLevel") {
    int powerLevelDelta = request_value["powerLevelDelta"];
    success = adjustPowerLevelCallback(device->getDeviceId(), powerLevelDelta);
    response_value["powerLevel"] = powerLevelDelta;
  }
  return success;
}

#endif