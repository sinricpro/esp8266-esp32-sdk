#pragma once

#include "../SinricProNamespace.h"
#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
namespace SINRICPRO_NAMESPACE {

FSTR(KEYPAD, SendKeystroke);  // "SendKeystroke"
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
    virtual bool onKeystroke(const String &keystroke);
    bool handleKeypadController(SinricProRequest &request);

  private:
    KeystrokeCallback keystrokeCallback;
};

template <typename T>
KeypadController<T>::KeypadController() {
    T *device = static_cast<T *>(this);
    device->registerRequestHandler(std::bind(&KeypadController<T>::handleKeypadController, this, std::placeholders::_1));
}

/**
 * @brief Set callback function for `SendKeystroke` request
 * 
 * @param cb Function pointer to a `KeystrokeCallback` function
 * @return void
 * @see KeystrokeCallback
 **/
template <typename T>
void KeypadController<T>::onKeystroke(KeystrokeCallback cb) {
    keystrokeCallback = cb;
}

template <typename T>
bool KeypadController<T>::onKeystroke(const String &keystroke) {
    T *device = static_cast<T *>(this);
    if (keystrokeCallback) return KeystrokeCallback(device->deviceId, keystroke);
    return false;
}

template <typename T>
bool KeypadController<T>::handleKeypadController(SinricProRequest &request) {
    bool success = false;

    if (request.action == FSTR_KEYPAD_SendKeystroke) {
        String keystroke = request.request_value[FSTR_KEYPAD_keystroke] | "";
        success = onKeystroke(keystroke);
        request.response_value[FSTR_KEYPAD_keystroke] = keystroke;
        return success;
    }

    return success;
}

}  // namespace SINRICPRO_NAMESPACE