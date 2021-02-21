#ifndef _KEYPADCONTROLLER_H_
#define _KEYPADCONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief KeypadController
 * @ingroup Capabilities
 **/
template <typename T>
class KeypadController {
  public:
    KeypadController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&KeypadController<T>::handleKeypadController, this, std::placeholders::_1)); }
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

    void onKeystroke(KeystrokeCallback cb);

  protected:
    bool handleKeypadController(SinricProRequest &request);

  private:
    KeystrokeCallback keystrokeCallback;
};

/**
 * @brief Set callback function for `SendKeystroke` request
 * 
 * @param cb Function pointer to a `KeystrokeCallback` function
 * @return void
 * @see KeystrokeCallback
 **/
template <typename T>
void KeypadController<T>::onKeystroke(KeystrokeCallback cb) { keystrokeCallback = cb; }


template <typename T>
bool KeypadController<T>::handleKeypadController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;
  if (request.action != "SendKeystroke") return false;

  if (keystrokeCallback) {
    String keystroke = request.request_value["keystroke"] | "";
    success = keystrokeCallback(device.deviceId, keystroke);
    request.response_value["keystroke"] = keystroke;
    return success;
  }
  
  return success;
}

#endif