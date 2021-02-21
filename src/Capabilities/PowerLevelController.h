#ifndef _POWERLEVELCONTROLLER_H_
#define _POWERLEVELCONTROLLER_H_

#include "./SinricProRequest.h"

/**
 * @brief PowerLevelController
 * @ingroup Capabilities
 **/
template <typename T>
class PowerLevelController {
  public:
    PowerLevelController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&PowerLevelController<T>::handlePowerLevelController, this, std::placeholders::_1)); }
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

  protected:
    bool handlePowerLevelController(SinricProRequest &request);

  private:
    SetPowerLevelCallback setPowerLevelCallback;
    AdjustPowerLevelCallback adjustPowerLevelCallback;
};

/**
 * @brief Set callback function for setPowerLevel request
 * 
 * @param     cb    Function pointer to a SetPowerLevelCallback function
 * @see       SetPowerLevelCallback
 **/
template <typename T>
void PowerLevelController<T>::onPowerLevel(SetPowerLevelCallback cb) {
  setPowerLevelCallback = cb;
}

/**
 * @brief Set callback function for adjustPowerLevel request
 * 
 * @param     cb    Function pointer to a AdjustPowerLevelCallback function
 * @see       AdjustPowerLevelCallback
 **/
template <typename T>
void PowerLevelController<T>::onAdjustPowerLevel(AdjustPowerLevelCallback cb)
{
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
template <typename T>
bool PowerLevelController<T>::sendPowerLevelEvent(int powerLevel, String cause)
{
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setPowerLevel", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["powerLevel"] = powerLevel;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool PowerLevelController<T>::handlePowerLevelController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (setPowerLevelCallback && request.action == "setPowerLevel") {
    int powerLevel = request.request_value["powerLevel"];
    success = setPowerLevelCallback(device.deviceId, powerLevel);
    request.response_value["powerLevel"] = powerLevel;
  }

  if (adjustPowerLevelCallback && request.action == "adjustPowerLevel") {
    int powerLevelDelta = request.request_value["powerLevelDelta"];
    success = adjustPowerLevelCallback(device.deviceId, powerLevelDelta);
    request.response_value["powerLevel"] = powerLevelDelta;
  }
  return success;
}

#endif