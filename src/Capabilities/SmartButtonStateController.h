#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"
#include "../SinricProNamespace.h"

namespace SINRICPRO_NAMESPACE {

// String constants for button states and actions
FSTR(BUTTONSTATE, state);               // Button state key
FSTR(BUTTONSTATE, singlePress);         // Single press state value
FSTR(BUTTONSTATE, doublePress);         // Double press state value
FSTR(BUTTONSTATE, longPress);           // Long press state value
FSTR(BUTTONSTATE, setSmartButtonState); // Set state action name

// Callback type definitions for different button press events
using SmartButtonPressCallback = std::function<bool(const String &)>;

/**
 * @brief Controller class for managing smart button state and interactions
 * 
 * @tparam T The device type that this controller is attached to
 */
template <typename T>
class SmartButtonStateController {
public:
    /**
     * @brief Construct a new Smart Button State Controller
     * Automatically registers the request handler with the device
     */
    SmartButtonStateController();

    /**
     * @brief Register callback for single press event from app
     * @param cb Callback function to handle single press
     */
    void onSinglePress(SmartButtonPressCallback cb);

    /**
     * @brief Register callback for double press event from app
     * @param cb Callback function to handle double press
     */
    void onDoublePress(SmartButtonPressCallback cb);

    /**
     * @brief Register callback for long press event from app
     * @param cb Callback function to handle long press
     */
    void onLongPress(SmartButtonPressCallback cb);

protected:
    /**
     * @brief Handle incoming button state change requests
     * @param request The incoming request to process
     * @return true if request was handled successfully, false otherwise
     */
    bool handleSmartButtonStateController(SinricProRequest &request);

private:
    SmartButtonPressCallback smartButtonSinglePressCallback;
    SmartButtonPressCallback smartButtonDoublePressCallback;
    SmartButtonPressCallback smartButtonLongPressCallback;

    /**
     * returns true if states match, false otherwise
     */
    inline bool isStateMatch(const SinricProRequest &request, const char* stateValue) {
        return request.request_value[FSTR_BUTTONSTATE_state] == stateValue;
    }
};

// Implementation

template <typename T>
SmartButtonStateController<T>::SmartButtonStateController() {
    T* device = static_cast<T*>(this);
    device->registerRequestHandler(
        std::bind(&SmartButtonStateController<T>::handleSmartButtonStateController, 
                 this, 
                 std::placeholders::_1)
    );
}

template <typename T>
void SmartButtonStateController<T>::onSinglePress(SmartButtonPressCallback cb) {
    smartButtonSinglePressCallback = cb;
}

template <typename T>
void SmartButtonStateController<T>::onDoublePress(SmartButtonPressCallback cb) {
    smartButtonDoublePressCallback = cb;
}

template <typename T>
void SmartButtonStateController<T>::onLongPress(SmartButtonPressCallback cb) {
    smartButtonLongPressCallback = cb;
}

template <typename T>
bool SmartButtonStateController<T>::handleSmartButtonStateController(SinricProRequest &request) {
    // Only process setSmartButtonState actions
    if (request.action != FSTR_BUTTONSTATE_setSmartButtonState) {
        return false;
    }

    T* device = static_cast<T*>(this);
    bool success = false;

    // Handle single press
    if (smartButtonSinglePressCallback && isStateMatch(request, FSTR_BUTTONSTATE_singlePress)) {
        success = smartButtonSinglePressCallback(device->deviceId);
    }
    // Handle double press
    else if (smartButtonDoublePressCallback && isStateMatch(request, FSTR_BUTTONSTATE_doublePress)) {
        success = smartButtonDoublePressCallback(device->deviceId);
    }
    // Handle long press
    else if (smartButtonLongPressCallback && isStateMatch(request, FSTR_BUTTONSTATE_longPress)) {
        success = smartButtonLongPressCallback(device->deviceId);
    }

    return success;
}

} // namespace SINRICPRO_NAMESPACE

template <typename T>
using SmartButtonStateController = SINRICPRO_NAMESPACE::SmartButtonStateController<T>;