#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(OPEN_CLOSE, openPercent);          // "openPercent"
FSTR(OPEN_CLOSE, openRelativePercent);  // "openRelativePercent"
FSTR(OPEN_CLOSE, openDirection);        // "openDirection"
FSTR(OPEN_CLOSE, setOpenClose);         // "setOpenClose" 
FSTR(OPEN_CLOSE, adjustOpenClose);      // "adjustOpenClose" 


/**
 * @brief Callback definition for onOpenClose callback
 * 
 * Gets called when device receives a `setOpenClose` request
 * @param[in]   deviceId      String containing the ID of device
 * @param[in,out] openPercent Integer percentage (0-100) of how open the device should be set (0 = closed, 100 = fully open)
 * @return      Success status of the request
 * @retval      true        Request handled successfully
 * @retval      false       Request handling failed
 * 
 * @section OpenCloseCallback Example-Code
 * @snippet callbacks.cpp onSetOpenClose
 **/
using OpenCloseCallback = std::function<bool(const String &, int &)>;

/**
 * @brief Callback definition for onAdjustOpenClose callback
 * 
 * Gets called when device receives an `adjustOpenClose` request
 * @param[in]   deviceId      String containing the ID of device
 * @param[in,out] openRelativePercent Integer value representing the relative percentage change to apply
 *                                  On output, should contain the absolute percentage value the device has been set to
 * @return      Success status of the request
 * @retval      true        Request handled successfully
 * @retval      false       Request handling failed
 * 
 * @section AdjustOpenCloseCallback Example-Code
 * @snippet callbacks.cpp onAdjustOpenClose
 **/
using AdjustOpenCloseCallback = std::function<bool(const String &, int &)>;

/**
 * @brief Callback definition for onDirectionOpenClose callback
 * 
 * Gets called when device receives a `setOpenClose` request with direction information
 * @param[in]   deviceId      String containing the ID of device
 * @param[in]   openDirection String direction in which to open: `UP`, `DOWN`, `LEFT`, `RIGHT`, `IN`, `OUT`
 * @param[in,out] openPercent Integer percentage (0-100) of how open the device should be set
 * @return      Success status of the request
 * @retval      true        Request handled successfully
 * @retval      false       Request handling failed
 * 
 * @section DirectionOpenCloseCallback Example-Code
 * @snippet callbacks.cpp onDirectionOpenClose
 **/
using DirectionOpenCloseCallback = std::function<bool(const String &, const String &, int &)>;

/**
 * @brief Callback definition for onAdjustDirectionOpenClose callback
 * 
 * Gets called when device receives an `adjustOpenClose` request with direction information
 * @param[in]   deviceId      String containing the ID of device
 * @param[in]   openDirection String direction in which to open: `UP`, `DOWN`, `LEFT`, `RIGHT`, `IN`, `OUT`
 * @param[in,out] openRelativePercent Integer representing relative percentage change to apply
 *                                  On output, should contain the absolute percentage value the device has been set to
 * @return      Success status of the request
 * @retval      true        Request handled successfully
 * @retval      false       Request handling failed
 * 
 * @section AdjustDirectionOpenCloseCallback Example-Code
 * @snippet callbacks.cpp onAdjustDirectionOpenClose
 **/
using AdjustDirectionOpenCloseCallback = std::function<bool(const String &, const String &, int &)>;

/**
 * @brief Controller class for devices with open/close functionality
 * @ingroup Capabilities
 * This controller handles open/close operations for devices that can be opened or closed by percentage and may supports multiple directions.
 * 
 * @tparam T Device type that implements this controller
 */
template <typename T>
class OpenCloseController {
  public:
    OpenCloseController();

    /**
     * @brief Set callback function for simple open/close operations
     * 
     * @param cb Function pointer to a `OpenCloseCallback` function
     * @return void
     * @see OpenCloseCallback
     **/
    void onOpenClose(OpenCloseCallback cb);

    /**
     * @brief Set callback function for directional open/close operations
     * 
     * @param cb Function pointer to a `DirectionOpenCloseCallback` function
     * @return void
     * @see DirectionOpenCloseCallback
     **/
    void onDirectionOpenClose(DirectionOpenCloseCallback cb);

    /**
     * @brief Set callback function for relative adjustments to open/close status
     * 
     * @param cb Function pointer to a `AdjustOpenCloseCallback` function
     * @return void
     * @see AdjustOpenCloseCallback
     **/
    void onAdjustOpenClose(AdjustOpenCloseCallback cb);

    /**
     * @brief Set callback function for directional relative adjustments
     * 
     * @param cb Function pointer to a `AdjustDirectionOpenCloseCallback` function
     * @return void
     * @see AdjustDirectionOpenCloseCallback
     **/
    void onAdjustDirectionOpenClose(AdjustDirectionOpenCloseCallback cb);

    /**
     * @brief Send an event to update the open/close status
     * 
     * @param openPercent Current open percentage (0-100)
     * @param cause Cause of the event (default: physical interaction)
     * @return bool Whether the event was sent successfully
     **/
    bool sendOpenCloseEvent(int openPercent, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

    /**
     * @brief Send an event to update the open/close status with direction
     * 
     * @param openDirection Direction of opening (UP, DOWN, LEFT, RIGHT, IN, OUT)
     * @param openPercent Current open percentage (0-100)
     * @param cause Cause of the event (default: physical interaction)
     * @return bool Whether the event was sent successfully
     **/
    bool sendOpenCloseEvent(String openDirection, int openPercent, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    /**
     * @brief Handle incoming open/close control requests
     * 
     * @param request The incoming request to process
     * @return bool Whether the request was handled successfully
     **/
    bool handleOpenCloseController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    DirectionOpenCloseCallback directionOpenCloseCallback;
    OpenCloseCallback openCloseCallback;
    AdjustOpenCloseCallback adjustOpenCloseCallback; 
    AdjustDirectionOpenCloseCallback adjustDirectionOpenCloseCallback;
};

template <typename T>
OpenCloseController<T>::OpenCloseController()
:event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&OpenCloseController<T>::handleOpenCloseController, this, std::placeholders::_1)); 
}

template <typename T>
void OpenCloseController<T>::onOpenClose(OpenCloseCallback cb) { openCloseCallback = cb; }
 
template <typename T>
void OpenCloseController<T>::onDirectionOpenClose(DirectionOpenCloseCallback cb) { directionOpenCloseCallback = cb; }

template <typename T>
void OpenCloseController<T>::onAdjustOpenClose(AdjustOpenCloseCallback cb) { adjustOpenCloseCallback = cb; }

template <typename T>
void OpenCloseController<T>::onAdjustDirectionOpenClose(AdjustDirectionOpenCloseCallback cb) { adjustDirectionOpenCloseCallback = cb; }

/**
 * @brief Send an event to update open/close status with open direction and precent and information
 * 
 * Sends the current open/close status with direction to the Sinric Pro cloud.
 * The event will only be sent if the event rate limiter allows it.
 * 
 * @param openDirection Direction in which the device is opening
 * @param openPercent Current open percentage (0-100)
 * @param cause Reason for the state change (default: physical interaction)
 * @return bool Whether the event was sent successfully
 */
template <typename T>
bool OpenCloseController<T>::sendOpenCloseEvent(String openDirection, int openPercent, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  JsonDocument eventMessage = device->prepareEvent(FSTR_OPEN_CLOSE_setOpenClose, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_OPEN_CLOSE_openDirection] = openDirection;
  event_value[FSTR_OPEN_CLOSE_openPercent] = openPercent;

  return device->sendEvent(eventMessage);
}

/**
 * @brief Send an event to update open/close status
 * 
 * Sends the current open/close percentage to the Sinric Pro cloud.
 * The event will only be sent if the event rate limiter allows it.
 * 
 * @param openPercent Current open percentage (0-100)
 * @param cause Reason for the state change (default: physical interaction)
 * @return bool Whether the event was sent successfully
 */
template <typename T>
bool OpenCloseController<T>::sendOpenCloseEvent(int openPercent, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  JsonDocument eventMessage = device->prepareEvent(FSTR_OPEN_CLOSE_setOpenClose, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_OPEN_CLOSE_openPercent] = openPercent;

  return device->sendEvent(eventMessage);
}

/**
 * @brief Handle incoming open/close requests
 * 
 * Processes both setOpenClose and adjustOpenClose requests with or without direction information.
 * Delegates to the appropriate callback function based on the request type and parameters.
 * 
 * @param request The incoming request containing action and parameters
 * @return bool Whether the request was handled successfully
 */
template <typename T>
bool OpenCloseController<T>::handleOpenCloseController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (request.action == FSTR_OPEN_CLOSE_setOpenClose) {
    bool hasOpenDirection = !request.request_value[FSTR_OPEN_CLOSE_openDirection].isNull();
    int openPercent = request.request_value[FSTR_OPEN_CLOSE_openPercent];

    if (hasOpenDirection && directionOpenCloseCallback) {
      String openDirection = request.request_value[FSTR_OPEN_CLOSE_openDirection];
      success = directionOpenCloseCallback(device->deviceId, openDirection, openPercent);
      request.response_value[FSTR_OPEN_CLOSE_openDirection] = openDirection;
      request.response_value[FSTR_OPEN_CLOSE_openPercent] = openPercent;
    } else if (!hasOpenDirection && openCloseCallback) {
      success = openCloseCallback(device->deviceId, openPercent);
      request.response_value[FSTR_OPEN_CLOSE_openPercent] = openPercent;
    }

    return success;
  }

  if (request.action == FSTR_OPEN_CLOSE_adjustOpenClose) {
    bool hasOpenDirection = !request.request_value[FSTR_OPEN_CLOSE_openDirection].isNull();
    int openRelativePercent = request.request_value[FSTR_OPEN_CLOSE_openRelativePercent];

    if (hasOpenDirection && adjustDirectionOpenCloseCallback) {
      String openDirection = request.request_value[FSTR_OPEN_CLOSE_openDirection];
      success = adjustDirectionOpenCloseCallback(device->deviceId, openDirection, openRelativePercent);
      request.response_value[FSTR_OPEN_CLOSE_openDirection] = openDirection;
      request.response_value[FSTR_OPEN_CLOSE_openPercent] = openRelativePercent;
    } else if (!hasOpenDirection && adjustOpenCloseCallback) {
      success = adjustOpenCloseCallback(device->deviceId, openRelativePercent);
      request.response_value[FSTR_OPEN_CLOSE_openPercent] = openRelativePercent;
    }

    return success;
  }

  return false;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using OpenCloseController = SINRICPRO_NAMESPACE::OpenCloseController<T>;