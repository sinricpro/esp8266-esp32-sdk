#pragma once

#include "../EventLimiter.h"
#include "../SinricProNamespace.h"
#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
namespace SINRICPRO_NAMESPACE {

FSTR(RANGE, setRangeValue);     // "setRangeValue"
FSTR(RANGE, rangeValue);        // "rangeValue"
FSTR(RANGE, adjustRangeValue);  // "adjustRangeValue"
FSTR(RANGE, rangeValueDelta);   // "rangeValueDelta"

/**
 * @brief Callback definition for onRangeValue function
 * 
 * Gets called when device receive a `setRangeValue` reuqest \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   rangeValue  for range value device has to be set
 * @param[out]  rangeValue  returning the current range value
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section SetRangeValueCallback Example-Code
 * @snippet callbacks.cpp onRangeValue
 **/

using SetRangeValueCallback = std::function<bool(const String &, int &)>;

/**
 * @brief Callback definition for onRangeValue function on a specific instance
 * 
 * Gets called when device receive a `setRangeValue` reuqest \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   instance    String instance name
 * @param[in]   rangeValue  for range value device has to be set
 * @param[out]  rangeValue  returning the current range value
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section GenericSetRangeValueCallback Example-Code
 * @snippet callbacks.cpp onRangeValueGeneric
 **/
using GenericSetRangeValueCallback = std::function<bool(const String &, const String &, float &)>;

/**
 * @brief Callback definition for onAdjustRangeValue function
 * 
 * Gets called when device receive a `adjustRangeValue` reuqest \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   rangeValue  delta value for range value have to change
 * @param[out]  rangeValue  returning the absolute range value 
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section AdjustRangeValueCallback Example-Code
 * @snippet callbacks.cpp onAdjustRangeValue
 **/
using AdjustRangeValueCallback = std::function<bool(const String &, int &)>;

/**
 * @brief Callback definition for onAdjustRangeValue function on a specific instance for custom devices
 * 
 * Gets called when device receive a `adjustRangeValue` reuqest \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   instance    String instance name
 * @param[in]   rangeValue  delta value for range value have to change
 * @param[out]  rangeValue  returning the absolute range value 
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section GenericAdjustRangeValueCallback Example-Code
 * @snippet callbacks.cpp onAdjustRangeValueGeneric
 **/
using GenericAdjustRangeValueCallback = std::function<bool(const String &, const String &, float &)>;

/**
 * @brief RangeControllerFloatInt
 * @ingroup Capabilities
 **/
template <typename T>
class RangeController : public SinricProRequestHandler {
  public:
    RangeController();

    void onRangeValue(SetRangeValueCallback cb);
    void onRangeValue(const String &instance, GenericSetRangeValueCallback cb);

    void onAdjustRangeValue(AdjustRangeValueCallback cb);
    void onAdjustRangeValue(const String &instance, GenericAdjustRangeValueCallback cb);

    bool sendRangeValueEvent(float rangeValue, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);
    bool sendRangeValueEvent(const String &instance, float rangeValue, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    virtual bool onRangeValue(int &value);
    virtual bool onRangeValue(const String &instance, float &value);
    virtual bool onAdjustRangeValue(int &valueDelta);
    virtual bool onAdjustRangeValue(const String &instance, float &valueDelta);

    bool handleRequest(SinricProRequest &request);

  private:
    EventLimiter                   event_limiter;
    std::map<String, EventLimiter> generic_event_limiter;

    SetRangeValueCallback                             setRangeValueCallback;
    std::map<String, GenericSetRangeValueCallback>    genericSetRangeValueCallbacks;
    AdjustRangeValueCallback                          adjustRangeValueCallback;
    std::map<String, GenericAdjustRangeValueCallback> genericAdjustRangeValueCallbacks;
};

template <typename T>
RangeController<T>::RangeController()
    : event_limiter(EVENT_LIMIT_STATE) {
    T *device = static_cast<T *>(this);
    device->registerRequestHandler(this);
}

/**
 * @brief Set callback function for `setRangeValue` request
 * 
 * @param cb Function pointer to a `SetRangeValueCallback` function
 * @see SetRangeValueCallback
 */
template <typename T>
void RangeController<T>::onRangeValue(SetRangeValueCallback cb) {
    setRangeValueCallback = cb;
}

/**
 * @brief Set callback function for `setRangeValue` request on a specific instance (custom device)
 * 
 * @param instance String instance name (custom device)
 * @param cb Function pointer to a `GenericSetRangeValueCallback` function
 * @see GenericSetRangeValueCallback
 */
template <typename T>
void RangeController<T>::onRangeValue(const String &instance, GenericSetRangeValueCallback cb) {
    genericSetRangeValueCallbacks[instance] = cb;
}

/**
 * @brief Set callback function for `adjustRangeValue` request
 * 
 * @param cb Function pointer to a `AdjustRangeValueCallback` function
 * @see AdjustRangeValueCallback
 */
template <typename T>
void RangeController<T>::onAdjustRangeValue(AdjustRangeValueCallback cb) {
    adjustRangeValueCallback = cb;
}

template <typename T>
void RangeController<T>::onAdjustRangeValue(const String &instance, GenericAdjustRangeValueCallback cb) {
    genericAdjustRangeValueCallbacks[instance] = cb;
}

template <typename T>
bool RangeController<T>::onRangeValue(int &value) {
    T *device = static_cast<T *>(this);
    if (setRangeValueCallback) return setRangeValueCallback(device->deviceId, value);
    return false;
}

template <typename T>
bool RangeController<T>::onRangeValue(const String &instance, float &value) {
    if (genericSetRangeValueCallbacks.find(instance) == genericSetRangeValueCallbacks.end()) return false;
    T *device = static_cast<T *>(this);

    bool success = genericSetRangeValueCallbacks[instance](device->deviceId, instance, value);
    return success;
}

template <typename T>
bool RangeController<T>::onAdjustRangeValue(int &valueDelta) {
    T *device = static_cast<T *>(this);
    if (adjustRangeValueCallback) return adjustRangeValueCallback(device->deviceId, valueDelta);
    return false;
}

template <typename T>
bool RangeController<T>::onAdjustRangeValue(const String &instance, float &valueDelta) {
    if (genericAdjustRangeValueCallbacks.find(instance) == genericAdjustRangeValueCallbacks.end()) return false;
    T *device = static_cast<T *>(this);

    bool success = genericAdjustRangeValueCallbacks[instance](device->deviceId, instance, valueDelta);
    return success;
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
template <typename T>
bool RangeController<T>::sendRangeValueEvent(float rangeValue, String cause) {
    if (event_limiter) return false;
    T *device = static_cast<T *>(this);

    DynamicJsonDocument eventMessage   = device->prepareEvent(FSTR_RANGE_setRangeValue, cause.c_str());
    JsonObject          event_value    = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
    event_value[FSTR_RANGE_rangeValue] = rangeValue;
    return device->sendEvent(eventMessage);
}

/**
 * @brief Send `rangeValue` event to report curent rangeValue to SinricPro server for a specific instance (custom device)
 * 
 * @param   instance    String instance name
 * @param   rangeValue  Value between 0..3
 * @param   cause       (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return  the success of sending the even
 * @retval  true        event has been sent successfully
 * @retval  false       event has not been sent, maybe you sent to much events in a short distance of time
 */
template <typename T>
bool RangeController<T>::sendRangeValueEvent(const String &instance, float rangeValue, String cause) {
    if (generic_event_limiter.find(instance) == generic_event_limiter.end()) generic_event_limiter[instance] = EventLimiter(EVENT_LIMIT_STATE);
    if (generic_event_limiter[instance]) return false;
    T *device = static_cast<T *>(this);

    DynamicJsonDocument eventMessage                                = device->prepareEvent(FSTR_RANGE_setRangeValue, cause.c_str());
    eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId] = instance;

    JsonObject event_value             = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
    event_value[FSTR_RANGE_rangeValue] = rangeValue;
    return device->sendEvent(eventMessage);
}

template <typename T>
bool RangeController<T>::handleRequest(SinricProRequest &request) {
    bool success = false;

    if (request.action == FSTR_RANGE_setRangeValue) {
        if (request.instance == "") {
            int rangeValue                                = request.request_value[FSTR_RANGE_rangeValue];
            success                                       = onRangeValue(rangeValue);
            request.response_value[FSTR_RANGE_rangeValue] = rangeValue;
            return success;

        } else {
            float rangeValue                              = request.request_value[FSTR_RANGE_rangeValue];
            success                                       = onRangeValue(request.instance, rangeValue);
            request.response_value[FSTR_RANGE_rangeValue] = rangeValue;
            return success;
        }
    }

    if (request.action == FSTR_RANGE_adjustRangeValue) {
        if (request.instance == "") {
            int rangeValue                                = request.request_value[FSTR_RANGE_rangeValueDelta];
            success                                       = onAdjustRangeValue(rangeValue);
            request.response_value[FSTR_RANGE_rangeValue] = rangeValue;
            return success;

        } else {
            float rangeValue                              = request.request_value[FSTR_RANGE_rangeValueDelta];
            success                                       = onAdjustRangeValue(request.instance, rangeValue);
            request.response_value[FSTR_RANGE_rangeValue] = rangeValue;
            return success;
        }
    }

    return false;
}

}  // namespace SINRICPRO_NAMESPACE