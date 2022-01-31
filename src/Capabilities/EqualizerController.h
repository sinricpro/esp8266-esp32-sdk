#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(EQUALIZER, setBands);          // "setBands"
FSTR(EQUALIZER, bands);             // "bands"
FSTR(EQUALIZER, name);              // "name"
FSTR(EQUALIZER, value);             // "value"
FSTR(EQUALIZER, level);             // "level"
FSTR(EQUALIZER, adjustBands);       // "adjustBands"
FSTR(EQUALIZER, levelDelta);        // "levelDelta"
FSTR(EQUALIZER, levelDirection);    // "levelDirection"
FSTR(EQUALIZER, DOWN);              // "DOWN"
FSTR(EQUALIZER, resetBands);        // "resetBands"

/**
 * @brief Callback definition for onSetBands function
 * 
 * Gets called when device receive a `setBands` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   bands       String with requested bands to change \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param[in]   level       Integer value with level bands should set to
 * @param[out]  bands       String with changed bands \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param[out]  level       Integer value with level bands changed to
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section SetBandsCallback Example-Code
 * @snippet callbacks.cpp onSetBands
 **/
using SetBandsCallback = std::function<bool(const String &, const String &, int &)>;

/**
 * @brief Callback definition for onAdjustBands function
 * 
 * Gets called when device receive a `adjustBands` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   bands       String with requested bands to change \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param[in]   levelDelta  Integer with relative level value device should change bands about
 * @param[out]  bands       String with changed bands \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param[out]  levelDelta  Integer value with level bands changed to
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section AdjustBandsCallback Example-Code
 * @snippet callbacks.cpp onAdjustBands
 **/
using AdjustBandsCallback = std::function<bool(const String &, const String &, int &)>;

/**
 * @brief Callback definition for onResetBands function
 * 
 * Gets called when device receive a `onResetBands` request \n
 * @param[in]   deviceId    String which contains the ID of device
 * @param[in]   bands       String with requested bands to reset \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param[out]  bands       String with changed bands \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param[out]  level       Integer value with level bands changed to
 * @return      the success of the request
 * @retval      true        request handled properly
 * @retval      false       request was not handled properly because of some error
 * 
 * @section ResetBandsCallback Example-Code
 * @snippet callbacks.cpp onResetBands
 **/
using ResetBandsCallback = std::function<bool(const String &, const String &, int &)>;

/**
 * @brief EqualizerController
 * @ingroup Capabilities
 **/
template <typename T>
class EqualizerController {
public:
  EqualizerController();

  void onSetBands(SetBandsCallback cb);
  void onAdjustBands(AdjustBandsCallback cb);
  void onResetBands(ResetBandsCallback cb);

  bool sendBandsEvent(String bands, int level, String cause = "PHYSICAL_INTERACTION");

protected:
  bool handleEqualizerController(SinricProRequest &request);

private:
  EventLimiter event_limiter;
  SetBandsCallback setBandsCallback;
  AdjustBandsCallback adjustBandsCallback;
  ResetBandsCallback resetBandsCallback;
};

template <typename T>
EqualizerController<T>::EqualizerController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&EqualizerController<T>::handleEqualizerController, this, std::placeholders::_1)); 
}

/**
 * @brief Set callback function for `setBands` request
 * 
 * @param cb Function pointer to a `SetBandsCallback` function
 * @return void
 * @see SetBandsCallback
 **/
template <typename T>
void EqualizerController<T>::onSetBands(SetBandsCallback cb) { setBandsCallback = cb; }

/**
 * @brief Set callback function for `adjustBands` request
 * 
 * @param cb Function pointer to a `AdjustBandsCallback` function
 * @return void
 * @see AdjustBandsCallback
 **/
template <typename T>
void EqualizerController<T>::onAdjustBands(AdjustBandsCallback cb) { adjustBandsCallback = cb; }

/**
 * @brief Set callback function for `resetBands` request
 * 
 * @param cb Function pointer to a `ResetBandsCallback` function
 * @return void
 * @see ResetBandsCallback
 **/
template <typename T>
void EqualizerController<T>::onResetBands(ResetBandsCallback cb) { resetBandsCallback = cb; }

/**
 * @brief Send `setBands` event to SinricPro Server indicating bands level has changed
 * 
 * @param bands   String which bands has changed \n `BASS`, `MIDRANGE`, `TREBBLE`
 * @param level   Integer with changed bands level
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool EqualizerController<T>::sendBandsEvent(String bands, int level, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent(FSTR_EQUALIZER_setBands, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  JsonArray event_value_bands = event_value.createNestedArray(FSTR_EQUALIZER_bands);
  JsonObject event_bands = event_value_bands.createNestedObject();
  event_bands[FSTR_EQUALIZER_name] = bands;
  event_bands[FSTR_EQUALIZER_value] = level;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool EqualizerController<T>::handleEqualizerController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);
  bool success = false;

  if (setBandsCallback && request.action == FSTR_EQUALIZER_setBands) {
    JsonArray bands_array = request.request_value[FSTR_EQUALIZER_bands];
    JsonArray response_value_bands = request.response_value.createNestedArray(FSTR_EQUALIZER_bands);

    for (size_t i = 0; i < bands_array.size(); i++) {
      int level = bands_array[i][FSTR_EQUALIZER_level] | 0;
      String bandsName = bands_array[i][FSTR_EQUALIZER_name] | "";
      success = setBandsCallback(device->deviceId, bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i[FSTR_EQUALIZER_name] = bandsName;
      response_value_bands_i[FSTR_EQUALIZER_level] = level;
    }
    return success;
  }

  if (adjustBandsCallback && request.action == FSTR_EQUALIZER_adjustBands) {
    JsonArray bands_array = request.request_value[FSTR_EQUALIZER_bands];
    JsonArray response_value_bands = request.response_value.createNestedArray(FSTR_EQUALIZER_bands);

    for (size_t i = 0; i < bands_array.size(); i++)  {
      int levelDelta = bands_array[i][FSTR_EQUALIZER_levelDelta] | 1;
      String direction = bands_array[i][FSTR_EQUALIZER_levelDirection];
      if (direction == FSTR_EQUALIZER_DOWN)
        levelDelta *= -1;
      String bandsName = bands_array[i][FSTR_EQUALIZER_name] | "";
      success = adjustBandsCallback(device->deviceId, bandsName, levelDelta);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i[FSTR_EQUALIZER_name] = bandsName;
      response_value_bands_i[FSTR_EQUALIZER_level] = levelDelta;
    }
    return success;
  }

  if (resetBandsCallback && request.action == FSTR_EQUALIZER_resetBands) {
    JsonArray bands_array = request.request_value[FSTR_EQUALIZER_bands];
    JsonArray response_value_bands = request.response_value.createNestedArray(FSTR_EQUALIZER_bands);

    for (size_t i = 0; i < bands_array.size(); i++) {
      int level = 0;
      String bandsName = bands_array[i][FSTR_EQUALIZER_name] | "";
      success = adjustBandsCallback(device->deviceId, bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i[FSTR_EQUALIZER_name] = bandsName;
      response_value_bands_i[FSTR_EQUALIZER_level] = level;
    }
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

template <typename T>
using EqualizerController = SINRICPRO_NAMESPACE::EqualizerController<T>;