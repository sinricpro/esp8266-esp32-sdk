#ifndef _MODECONTROLLER_H_
#define _MODECONTROLLER_H_

#include "SinricProRequest.h"

/**
 * @brief ModeController
 * @ingroup Capabilities
 **/
template <typename T>
class ModeController {
  public:
    ModeController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&ModeController<T>::handleModeController, this, std::placeholders::_1)); }
    /**
     * @brief Callback definition for onSetMode function
     * 
     * Gets called when device receive a `setBands` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   mode        String device mode should set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
     * @param[out]  mode        String devices mode is set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section ModeCallback Example-Code
     * @snippet callbacks.cpp onSetMode
     **/
    using ModeCallback = std::function<bool(const String &, String &)>;

    /**
     * @brief Callback definition for onSetMode function for a specific instance
     * 
     * Gets called when device receive a `setBands` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   instance    String name of the instance
     * @param[in]   mode        String device mode should set to
     * @param[out]  mode        String devices mode is set to
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * 
     * @section GenericModeCallback Example-Code
     * @snippet callbacks.cpp onSetModeGeneric
     **/
    using GenericModeCallback = std::function<bool(const String &, const String &, String &)>;

    void onSetMode(ModeCallback cb);
    void onSetMode(const String& instance, GenericModeCallback cb);

    bool sendModeEvent(String mode, String cause = "PHYSICAL_INTERACTION");
    bool sendModeEvent(String instance, String mode, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleModeController(SinricProRequest &request);

  private:
    ModeCallback setModeCallback;
    std::map<String, GenericModeCallback> genericModeCallback;
};

/**
 * @brief Set callback function for `setMode` request
 * 
 * @param cb Function pointer to a `ModeCallback` function
 * @return void
 * @see ModeCallback
 **/
template <typename T>
void ModeController<T>::onSetMode(ModeCallback cb) { setModeCallback = cb; }

/**
 * @brief Set callback function for `setMode` request on a specific instance
 * 
 * @param instance String with instance name
 * @param cb Function pointer to a `ModeCallback` function
 * @return void
 * @see ModeCallback
 **/
template <typename T>
void ModeController<T>::onSetMode(const String& instance, GenericModeCallback cb) {
  genericModeCallback[instance] = cb;
}

/**
 * @brief Send `setMode` event to SinricPro Server indicating the mode has changed
 * 
 * @param mode    String with actual mode device is set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ModeController<T>::sendModeEvent(String mode, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setMode", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mode"] = mode;
  return device.sendEvent(eventMessage);
}

/**
 * @brief Send `setMode` event to SinricPro Server indicating the mode on a specific instance has changed
 * 
 * @param instance String instance name
 * @param mode    String with actual mode device is set to \n `MOVIE`, `MUSIC`, `NIGHT`, `SPORT`, `TV`
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ModeController<T>::sendModeEvent(String instance, String mode, String cause) {
  T &device = static_cast<T &>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setMode", cause.c_str());
  eventMessage["payload"]["instanceId"] = instance;
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["mode"] = mode;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool ModeController<T>::handleModeController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;
  if (request.action != "setMode") return false;
  String mode = request.request_value["mode"] | "";

  if (request.instance != "") {
    if (genericModeCallback.find(request.instance) != genericModeCallback.end()) {
      success = genericModeCallback[request.instance](device.deviceId, request.instance, mode);
      request.response_value["mode"] = mode;
      return success;
    } else return false;
  } else {
    if (setModeCallback) {
      success = setModeCallback(device.deviceId, mode);
      request.response_value["mode"] = mode;
      return success;
    }
  }
  
  return success;
}

#endif