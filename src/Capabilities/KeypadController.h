#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(KEYPAD, SendKeystroke);
FSTR(KEYPAD, keystroke);

using KeystrokeCallback = std::function<bool(const String &, String &)>;

template <typename T>
class KeypadController {
  public:
    KeypadController();
    void onKeystroke(KeystrokeCallback cb);

  protected:
    virtual bool onKeystroke(String &keyStroke);
    bool handleKeypadController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    KeystrokeCallback keystrokeCallback;
};

template <typename T>
KeypadController<T>::KeypadController() 
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&KeypadController<T>::handleKeypadController, this, std::placeholders::_1)); 
}

template <typename T>
void KeypadController<T>::onKeystroke(KeystrokeCallback cb) {
  keystrokeCallback = cb;
}

template <typename T>
bool KeypadController<T>::onKeystroke(String &keyStroke) { 
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);
  if (keystrokeCallback) return keystrokeCallback(device->deviceId, keyStroke);
  return false;
}

template <typename T>
bool KeypadController<T>::handleKeypadController(SinricProRequest &request) {
  if (request.action != FSTR_KEYPAD_SendKeystroke) return false;

  String keystroke = request.request_value[FSTR_KEYPAD_keystroke] | "";
  bool success = onKeystroke(keystroke);
  request.response_value[FSTR_KEYPAD_keystroke] = keystroke;
  return success;
}

} // SINRICPRO_NAMESPACE