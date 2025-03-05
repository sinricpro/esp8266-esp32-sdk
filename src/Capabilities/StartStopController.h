#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(START_STOP, start);            // "start"
FSTR(START_STOP, pause);            // "pause"
FSTR(START_STOP, setStartStop);     // "setStartStop"
FSTR(START_STOP, setPauseUnpause);  // "setPauseUnpause"

/**
 * @brief Definition for onStartStop callback
 * 
 * Gets called when device receive a `setStartStop` reuqest \n
 * @param[in]     deviceId        String which contains the ID of device
 * @param[in]     start           true for start, false for stop
 * @return        the success of the request
 * @retval        true            request handled properly
 * @retval        false           request was not handled properly because of some error
 * 
 * @section StartStopCallback Example-Code
 * @snippet callbacks.cpp onStartStop
 **/
using StartStopCallback = std::function<bool(const String &, bool &)>;

/**
 * @brief Definition for onPauseUnpause callback
 * 
 * Gets called when device receive a `setPauseUnpause` reuqest \n
 * @param[in]     deviceId        String which contains the ID of device
 * @param[in]     pause           true for pause, false for unpause
 * @return        the success of the request
 * @retval        true            request handled properly
 * @retval        false           request was not handled properly because of some error
 * 
 * @section StartStopCallback Example-Code
 * @snippet callbacks.cpp onPauseUnpause
 **/

using PauseUnpauseCallback = std::function<bool(const String &, bool &)>;

/**
 * @brief StartStopController class to handle start/stop and pause/unpause functionality
 * 
 * This template class provides functionality to control devices that can be started,
 * stopped, paused, and unpaused (like a vacuum cleaner, washing machine, etc.)
 * 
 * @tparam T The device type that this controller is attached to
 */
template <typename T>
class StartStopController {
  public:
    StartStopController();

    /**
     * @brief Set the callback function for start/stop events
     * 
     * @param cb Callback function that will be called when a start/stop request is received
     */
    void onStartStop(StartStopCallback cb);

    /**
     * @brief Set the callback function for pause/unpause events
     * 
     * @param cb Callback function that will be called when a pause/unpause request is received
     */
    void onPauseUnpause(StartStopCallback cb);

    /**
     * @brief Send a start/stop event to the SinricPro server
     * 
     * @param start true for start, false for stop
     * @param cause The cause of the event (default: "PHYSICAL_INTERACTION")
     * @return true if event was sent successfully, false otherwise
     */
    bool sendStartStopEvent(bool start, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

    /**
     * @brief Send a pause/unpause event to the SinricPro server
     * 
     * @param pause true for pause, false for unpause
     * @param cause The cause of the event (default: "PHYSICAL_INTERACTION")
     * @return true if event was sent successfully, false otherwise
     */
    bool sendPauseUnpauseEvent(bool pause, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handleStartStopController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    StartStopCallback startStopCallbackCallback;
    PauseUnpauseCallback pauseUnpauseCallback;
};

template <typename T>
StartStopController<T>::StartStopController()
:event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&StartStopController<T>::handleStartStopController, this, std::placeholders::_1)); 
}

template <typename T>
void StartStopController<T>::onStartStop(StartStopCallback cb) { startStopCallbackCallback = cb; }

template <typename T>
void StartStopController<T>::onPauseUnpause(PauseUnpauseCallback cb) { pauseUnpauseCallback = cb; }

/**
 * @brief Send a start/stop event to the SinricPro server
 * 
 * Creates a JSON message with the start status and sends it to the server
 * @param   start       `bool` true for start, false for stop
 * @param   cause       (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`) 
 */
template <typename T>
bool StartStopController<T>::sendStartStopEvent(bool start, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  JsonDocument eventMessage = device->prepareEvent(FSTR_START_STOP_setStartStop, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_START_STOP_start] = start;
  return device->sendEvent(eventMessage);
}

/**
 * @brief Send a pause/unpause event to the SinricPro server
 * 
 * Creates a JSON message with the pause status and sends it to the server
 * @param   pause       `bool` true for pause, false for unpause
 * @param   cause       (optional) Reason why event is sent (default = `"PHYSICAL_INTERACTION"`) 
 */
template <typename T>
bool StartStopController<T>::sendPauseUnpauseEvent(bool pause, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  JsonDocument eventMessage = device->prepareEvent(FSTR_START_STOP_setPauseUnpause, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_START_STOP_pause] = pause;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool StartStopController<T>::handleStartStopController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (startStopCallbackCallback && request.action == FSTR_START_STOP_setStartStop) {
    bool start = request.request_value[FSTR_START_STOP_start];
    success = startStopCallbackCallback(device->deviceId, start);
    request.response_value[FSTR_START_STOP_start] = start;
    return success;
  }
  
  if (pauseUnpauseCallback && request.action == FSTR_START_STOP_setPauseUnpause) {
    bool pause = request.request_value[FSTR_START_STOP_pause];
    success = pauseUnpauseCallback(device->deviceId, pause);
    request.response_value[FSTR_START_STOP_pause] = pause;
    return success;
  }

  return false;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using StartStopController = SINRICPRO_NAMESPACE::StartStopController<T>;