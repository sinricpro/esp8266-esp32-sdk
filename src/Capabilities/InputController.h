#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using SelectInputCallback = std::function<bool(const String &, String &)>;

template <typename T>
class InputController {
  public:
    InputController();

    void onSelectInput(SelectInputCallback cb);
    bool sendSelectInputEvent(String intput, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onSelectInput(String &input);

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

template <typename T>
void InputController<T>::onSelectInput(SelectInputCallback cb) {
  selectInputCallback = cb;
}

template <typename T>
bool InputController<T>::onSelectInput(String &input) {
  T* device = static_cast<T*>(this);
  if (selectInputCallback) return selectInputCallback(device->deviceId, input);
  return false;
}

template <typename T>
bool InputController<T>::sendSelectInputEvent(String input, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("selectInput", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["input"] = input;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool InputController<T>::handleInputController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "selectInput") {
    String input = request.request_value["input"];
    success = onSelectInput(input);
    request.response_value["input"] = input;
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE