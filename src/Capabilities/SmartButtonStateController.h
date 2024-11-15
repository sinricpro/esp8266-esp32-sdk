#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"
#include "../SinricProNamespace.h"

/**
 * @brief Enum defining the different types of button press events
 */
enum class SmartButtonPressType {
    SINGLE_PRESS,
    DOUBLE_PRESS,
    LONG_PRESS
};

namespace SINRICPRO_NAMESPACE {

FSTR(BUTTONSTATE, state);               // Button state key
FSTR(BUTTONSTATE, singlePress);         // Single press state value
FSTR(BUTTONSTATE, doublePress);         // Double press state value
FSTR(BUTTONSTATE, longPress);           // Long press state value
FSTR(BUTTONSTATE, setSmartButtonState); // Set state action name

// Callback type definition for button press events
using SmartButtonPressCallback = std::function<bool(const String&, SmartButtonPressType)>;

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
     * @brief Register callback for button press events from the app
     * @param cb Callback function to handle button press events
     */
    void onButtonPress(SmartButtonPressCallback cb);

protected:
    /**
     * @brief Handle incoming button state change requests
     * @param request The incoming request to process
     * @return true if request was handled successfully, false otherwise
     */
    bool handleSmartButtonStateController(SinricProRequest &request);

private:
    SmartButtonPressCallback buttonPressCallback;

    /**
     * @brief Convert string state to SmartButtonPressType enum
     * @param stateStr The state string from the request
     * @return corresponding SmartButtonPressType enum value
     */
    SmartButtonPressType getSmartButtonPressType(const String& stateStr);
};

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
void SmartButtonStateController<T>::onButtonPress(SmartButtonPressCallback cb) {
    buttonPressCallback = cb;
}

template <typename T>
SmartButtonPressType SmartButtonStateController<T>::getSmartButtonPressType(const String& stateStr) {
    if (stateStr == FSTR_BUTTONSTATE_singlePress) {
        return SmartButtonPressType::SINGLE_PRESS;
    } else if (stateStr == FSTR_BUTTONSTATE_doublePress) {
        return SmartButtonPressType::DOUBLE_PRESS;
    } else {
        return SmartButtonPressType::LONG_PRESS;
    }
}

template <typename T>
bool SmartButtonStateController<T>::handleSmartButtonStateController(SinricProRequest &request) {
    // Only process setSmartButtonState actions
    if (request.action != FSTR_BUTTONSTATE_setSmartButtonState || !buttonPressCallback) {
        return false;
    }

    T* device = static_cast<T*>(this);
    String stateStr = request.request_value[FSTR_BUTTONSTATE_state];
    
    // Only process valid button states
    if (stateStr != FSTR_BUTTONSTATE_singlePress && 
        stateStr != FSTR_BUTTONSTATE_doublePress && 
        stateStr != FSTR_BUTTONSTATE_longPress) {
        return false;
    }

    SmartButtonPressType pressType = getSmartButtonPressType(stateStr);
    return buttonPressCallback(device->deviceId, pressType);
}

} // namespace SINRICPRO_NAMESPACE

template <typename T>
using SmartButtonStateController = SINRICPRO_NAMESPACE::SmartButtonStateController<T>;