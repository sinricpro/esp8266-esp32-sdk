#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(PERCENTAGE, setPercentage);      // "setPercentage"
FSTR(PERCENTAGE, percentage);         // "percentage"
FSTR(PERCENTAGE, adjustPercentage);   // "adjustPercentage"

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


/**
 * @brief PercentageController
 * @ingroup Capabilities
 **/
template <typename T>
class PercentageController {
  public:
    PercentageController();

    void onSetPercentage(SetPercentageCallback cb);
    void onAdjustPercentage(AdjustPercentageCallback cb);

    bool sendSetPercentageEvent(int percentage, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handlePercentageController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    SetPercentageCallback percentageCallback;
    AdjustPercentageCallback adjustPercentageCallback;
};

template <typename T>
PercentageController<T>::PercentageController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&PercentageController<T>::handlePercentageController, this, std::placeholders::_1)); 
}

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
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_PERCENTAGE_setPercentage, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_PERCENTAGE_percentage] = percentage;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool PercentageController<T>::handlePercentageController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (percentageCallback && request.action == FSTR_PERCENTAGE_setPercentage) {
    int percentage = request.request_value[FSTR_PERCENTAGE_percentage];
    success = percentageCallback(device->deviceId, percentage);
    request.response_value[FSTR_PERCENTAGE_percentage] = percentage;
    return success;
  }

  if (adjustPercentageCallback && request.action == FSTR_PERCENTAGE_adjustPercentage) {
    int percentage = request.request_value[FSTR_PERCENTAGE_percentage];
    success = adjustPercentageCallback(device->deviceId, percentage);
    request.response_value[FSTR_PERCENTAGE_percentage] = percentage;
    return success;
  }
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using PercentageController = SINRICPRO_NAMESPACE::PercentageController<T>;