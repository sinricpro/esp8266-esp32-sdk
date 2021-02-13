#ifndef _KEYPADCONTROLLER_H_
#define _KEYPADCONTROLLER_H_

/**
 * @brief KeypadController
 * @ingroup Controller
 **/
template <typename T>
class KeypadController {
  public:
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
    bool handleKeypadController(const String &action, JsonObject &request_value, JsonObject &response_value);

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
bool KeypadController<T>::handleKeypadController(const String &action, JsonObject &request_value, JsonObject &response_value) {
  T &device = static_cast<T &>(*this);

  bool success = false;
  if (action != "SendKeystroke") return false;
  String keystroke = request_value["keystroke"] | "";

  if (keystrokeCallback) {
    success = keystrokeCallback(device.deviceId, keystroke);
    response_value["keystroke"] = keystroke;
    return success;
  }
  
  return success;
}

#endif