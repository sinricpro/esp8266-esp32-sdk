#pragma once

#include "../SinricProRequest.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(KEYPAD, sendKeystroke);  // "sendKeystroke"
FSTR(KEYPAD, keystroke);      // "keystroke"

/**
 * @brief Callback definition for onKeystroke function
 * 
 * Gets called when device receive a `setBands` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   keystroke   String keystroke \n `INFO`, `MORE`, `SELECT`, `UP`, `DOWN`, `LEFT`, `RIGHT`, `PAGE_UP`, `PAGE_DOWN`, `PAGE_LEFT`, `PAGE_RIGHT`
 * @param[out]  keystroke   String keystroke \n `INFO`, `MORE`, `SELECT`, `UP`, `DOWN`, `LEFT`, `RIGHT`, `PAGE_UP`, `PAGE_DOWN`, `PAGE_LEFT`, `PAGE_RIGHT`
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section KeystrokeCallback Example-Code
 * @snippet callbacks.cpp onKeystroke
 **/
using KeystrokeCallback = std::function<bool(const String &, String &)>;


/**
 * @brief KeypadController
 * @ingroup Capabilities
 **/
template <typename T>
class KeypadController {
  public:
    KeypadController();

    void onKeystroke(KeystrokeCallback cb);

  protected:
    bool handleKeypadController(SinricProRequest &request);

  private:
    KeystrokeCallback keystrokeCallback;
};

template <typename T>
KeypadController<T>::KeypadController() {
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&KeypadController<T>::handleKeypadController, this, std::placeholders::_1)); 
}

/**
 * @brief Set callback function for `sendKeystroke` request
 * 
 * @param cb Function pointer to a `KeystrokeCallback` function
 * @return void
 * @see KeystrokeCallback
 **/
template <typename T>
void KeypadController<T>::onKeystroke(KeystrokeCallback cb) { keystrokeCallback = cb; }


template <typename T>
bool KeypadController<T>::handleKeypadController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;
  if (request.action != FSTR_KEYPAD_sendKeystroke) return false;

  if (keystrokeCallback) {
    String keystroke = request.request_value[FSTR_KEYPAD_keystroke] | "";
    success = keystrokeCallback(device->deviceId, keystroke);
    request.response_value[FSTR_KEYPAD_keystroke] = keystroke;
    return success;
  }
  
  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using KeypadController = SINRICPRO_NAMESPACE::KeypadController<T>;