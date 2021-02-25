#ifndef _PERCENTAGECONTROLLER_H_
#define _PERCENTAGECONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief PercentageController
 * @ingroup Capabilities
 **/
template <typename T>
class PercentageController {
  public:
    PercentageController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&PercentageController<T>::handlePercentageController, this, std::placeholders::_1)); }
    /**
     * @brief Callback definition for onSetPercentage function
     * 
     * Gets called when device receive a `setPercentage` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   percentage      Integer with percentage device should set to
     * @param[out]  percentage      Integer with percentage device has been set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section SetPercentageCallback Example-Code
     * @snippet callbacks.cpp onSetPercentage
     **/
    using SetPercentageCallback = std::function<bool(const String &, int &)>;

    /**
     * @brief Callback definition for onAdjustPercentage function
     * 
     * Gets called when device receive a `adjustPercentage` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   percentageDelta Integer with relative percentage the device should change about (-100..100)
     * @param[out]  percentageDelta Integer with absolute percentage device has been set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section AdjustPercentageCallback Example-Code
     * @snippet callbacks.cpp onAdjustPercentage
     **/
    using AdjustPercentageCallback = std::function<bool(const String &, int &)>;

    void onSetPercentage(SetPercentageCallback cb);
    void onAdjustPercentage(AdjustPercentageCallback cb);

    bool sendSetPercentageEvent(int percentage, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handlePercentageController(SinricProRequest &request);

  private:
    SetPercentageCallback percentageCallback;
    AdjustPercentageCallback adjustPercentageCallback;
};

/**
 * @brief Set callback function for `setPercentage` request
 * 
 * @param cb Function pointer to a `SetPercentageCallback` function
 * @return void
 * @see SetPercentageCallback
 **/
template <typename T>
void PercentageController<T>::onSetPercentage(SetPercentageCallback cb) { percentageCallback = cb; }

/**
 * @brief Set callback function for `adjustPercentage` request
 * 
 * @param cb Function pointer to a `AdjustPercentageCallback` function
 * @return void
 * @see AdjustPercentageCallback
 **/
template <typename T>
void PercentageController<T>::onAdjustPercentage(AdjustPercentageCallback cb) { adjustPercentageCallback = cb; }

/**
 * @brief Send `setPercentage` event to SinricPro Server indicating actual percentage has changed
 * 
 * @param   percentage        Integer reporting the percentage that the device have been set to
 * @param   cause         (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool PercentageController<T>::sendSetPercentageEvent(int percentage, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setPercentage", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["percentage"] = percentage;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool PercentageController<T>::handlePercentageController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (percentageCallback && request.action == "setPercentage") {
    int percentage = request.request_value["percentage"];
    success = percentageCallback(device.deviceId, percentage);
    request.response_value["percentage"] = percentage;
    return success;
  }

  if (adjustPercentageCallback && request.action == "adjustPercentage") {
    int percentage = request.request_value["percentage"];
    success = adjustPercentageCallback(device.deviceId, percentage);
    request.response_value["percentage"] = percentage;
    return success;
  }
  return success;
}

#endif