#ifndef _RANGECONTROLLER_H_
#define _RANGECONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

/**
 * @class RangeController
 * @brief supports onSetRangeValue, onAdjustRangeValue, sendRangeValueEvent
 **/
class RangeController {
  public:
    RangeController(SinricProDeviceInterface* device);

    /**
     * @brief Callback definition for onRangeValue function
     * 
     * Gets called when device receive a `setRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer 0..3 for range value device has to be set
     * @param[out]  rangeValue  Integer 0..3 returning the current range value
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SetRangeValueCallback Example-Code
     * @snippet callbacks.cpp onRangeValue
     **/
    using SetRangeValueCallback = std::function<bool(const String &, int &)>;

    /**
     * @brief Callback definition for onAdjustRangeValue function
     * 
     * Gets called when device receive a `adjustRangeValue` reuqest \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   rangeValue  Integer -3..3 delta value for range value have to change
     * @param[out]  rangeValue  Integer 3..3 returning the absolute range value 
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustRangeValueCallback Example-Code
     * @snippet callbacks.cpp onAdjustRangeValue
     **/
    using AdjustRangeValueCallback = std::function<bool(const String &, int &)>;

    void onRangeValue(SetRangeValueCallback cb);
    void onAdjustRangeValue(AdjustRangeValueCallback cb);

    bool sendRangeValueEvent(int rangeValue, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);
    
  private:
    SinricProDeviceInterface *device;
    SetRangeValueCallback setRangeValueCallback;
    AdjustRangeValueCallback adjustRangeValueCallback;
};

RangeController::RangeController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Set callback function for `setRangeValue` request
 * 
 * @param cb Function pointer to a `SetRangeValueCallback` function
 * @see SetRangeValueCallback
 */
void RangeController::onRangeValue(SetRangeValueCallback cb) {
  setRangeValueCallback = cb;
}

/**
 * @brief Set callback function for `adjustRangeValue` request
 * 
 * @param cb Function pointer to a `AdjustRangeValueCallback` function
 * @see AdjustRangeValueCallback
 */
void RangeController::onAdjustRangeValue(AdjustRangeValueCallback cb) {
  adjustRangeValueCallback = cb;
}

/**
 * @brief Send `rangeValue` event to report curent rangeValue to SinricPro server
 * 
 * @param   rangeValue  Value between 0..3
 * @param   cause       (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true        event has been sent successfully
 * @retval  false       event has not been sent, maybe you sent to much events in a short distance of time
 */
bool RangeController::sendRangeValueEvent(int rangeValue, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("setRangeValue", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return device->sendEvent(eventMessage);
}

bool RangeController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (actionString == "setRangeValue" && setRangeValueCallback)
  {
    int rangeValue = request_value["rangeValue"] | 0;
    success = setRangeValueCallback(device->getDeviceId(), rangeValue);
    response_value["rangeValue"] = rangeValue;
    return success;
  }

  if (actionString == "adjustRangeValue" && adjustRangeValueCallback)
  {
    int rangeValueDelta = request_value["rangeValueDelta"] | 0;
    success = adjustRangeValueCallback(device->getDeviceId(), rangeValueDelta);
    response_value["rangeValue"] = rangeValueDelta;
    return success;
  }

  return success;
}

#endif