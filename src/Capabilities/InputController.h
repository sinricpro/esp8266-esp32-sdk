#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(INPUT, selectInput);     // "selectInput"
FSTR(INPUT, input);           // "input"

/**
 * @brief Callback definition for onSelectInput function
 * 
 * Gets called when device receive a `selectInput` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   input       String with input name device is requested to switch to \n `AUX 1`..`AUX 7`, `BLURAY`, `CABLE`, `CD`, `COAX 1`,`COAX 2`, `COMPOSITE 1`, `DVD`, `GAME`, `HD RADIO`, `HDMI 1`.. `HDMI 10`, `HDMI ARC`, `INPUT 1`..`INPUT 10`, `IPOD`, `LINE 1`..`LINE 7`, `MEDIA PLAYER`, `OPTICAL 1`, `OPTICAL 2`, `PHONO`, `PLAYSTATION`, `PLAYSTATION 3`, `PLAYSTATION 4`, `SATELLITE`, `SMARTCAST`, `TUNER`, `TV`, `USB DAC`, `VIDEO 1`..`VIDEO 3`, `XBOX`
 * @param[out]  input       String with input name device has switchted to \n `AUX 1`..`AUX 7`, `BLURAY`, `CABLE`, `CD`, `COAX 1`,`COAX 2`, `COMPOSITE 1`, `DVD`, `GAME`, `HD RADIO`, `HDMI 1`.. `HDMI 10`, `HDMI ARC`, `INPUT 1`..`INPUT 10`, `IPOD`, `LINE 1`..`LINE 7`, `MEDIA PLAYER`, `OPTICAL 1`, `OPTICAL 2`, `PHONO`, `PLAYSTATION`, `PLAYSTATION 3`, `PLAYSTATION 4`, `SATELLITE`, `SMARTCAST`, `TUNER`, `TV`, `USB DAC`, `VIDEO 1`..`VIDEO 3`, `XBOX`
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section SelectInput Example-Code
 * @snippet callbacks.cpp onSelectInput
 **/
using SelectInputCallback = std::function<bool(const String &, String &)>;


/**
 * @brief InputController
 * @ingroup Capabilities
 **/
template <typename T>
class InputController {
  public:
    InputController();

    void onSelectInput(SelectInputCallback cb);
    bool sendSelectInputEvent(String intput, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handleInputController(SinricProRequest &request);

  private: 
    EventLimiter event_limiter;
    SelectInputCallback selectInputCallback;
};

template <typename T>
InputController<T>::InputController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&InputController<T>::handleInputController, this, std::placeholders::_1)); 
}

/**
 * @brief Set callback function for `selectInput` request
 * 
 * @param cb Function pointer to a `SelectInputCallback` function
 * @return void
 * @see SelectInputCallback
 **/
template <typename T>
void InputController<T>::onSelectInput(SelectInputCallback cb) {
  selectInputCallback = cb;
}

/**
 * @brief Send `selectInput` event to SinricPro Server to report selected input
 * 
 * @param input           String with actual media control \n `AUX 1`..`AUX 7`, `BLURAY`, `CABLE`, `CD`, `COAX 1`,`COAX 2`, `COMPOSITE 1`, `DVD`, `GAME`, `HD RADIO`, `HDMI 1`.. `HDMI 10`, `HDMI ARC`, `INPUT 1`..`INPUT 10`, `IPOD`, `LINE 1`..`LINE 7`, `MEDIA PLAYER`, `OPTICAL 1`, `OPTICAL 2`, `PHONO`, `PLAYSTATION`, `PLAYSTATION 3`, `PLAYSTATION 4`, `SATELLITE`, `SMARTCAST`, `TUNER`, `TV`, `USB DAC`, `VIDEO 1`..`VIDEO 3`, `XBOX`
 * @param cause           (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool InputController<T>::sendSelectInputEvent(String input, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_INPUT_selectInput, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  event_value[FSTR_INPUT_input] = input;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool InputController<T>::handleInputController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (selectInputCallback && request.action == FSTR_INPUT_selectInput) {
    String input = request.request_value[FSTR_INPUT_input];
    success = selectInputCallback(device->deviceId, input);
    request.response_value[FSTR_INPUT_input] = input;
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using InputController = SINRICPRO_NAMESPACE::InputController<T>;