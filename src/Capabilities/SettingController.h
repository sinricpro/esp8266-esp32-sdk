#pragma once

#include <variant>

#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(SETTING, setSetting);     // "setSetting"
FSTR(SETTING, id);             // "id"
FSTR(SETTING, value);          // "value"

/**
 * @brief Variant type for setting values that can hold int, float, bool, or String
 */
using SettingValue = std::variant<int, float, bool, String>;

using SetSettingCallback = std::function<bool(const String&, const String&, SettingValue&)>;

template <typename T>
class SettingController {
  public:
    SettingController();
    void onSetSetting(SetSettingCallback cb);
    bool sendSettingEvent(String settingId, SettingValue settingValue, String cause = FSTR_SINRICPRO_PHYSICAL_INTERACTION);

  protected:
    bool handleSettingController(SinricProRequest &request);

  private:
    EventLimiter event_limiter;
    SetSettingCallback setSettingCallback;
};

template <typename T>
SettingController<T>::SettingController()
: event_limiter(EVENT_LIMIT_STATE) {
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&SettingController<T>::handleSettingController, this, std::placeholders::_1));
}

template <typename T>
void SettingController<T>::onSetSetting(SetSettingCallback cb) {
  setSettingCallback = cb;
}

/**
 * @brief Send `setSetting` event to SinricPro Server
 *
 * @param settingId   `String` the setting identifier
 * @param settingValue `SettingValue` (int, float, bool, or String) the setting value
 * @param cause       (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the event
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent too many events in a short distance of time
 **/
template <typename T>
bool SettingController<T>::sendSettingEvent(String settingId, SettingValue settingValue, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  JsonDocument eventMessage = device->prepareEvent(FSTR_SETTING_setSetting, cause.c_str());
  JsonObject event_value = eventMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  eventMessage[FSTR_SINRICPRO_scope] = FSTR_SINRICPRO_device;
  event_value[FSTR_SETTING_id] = settingId;

  if (std::holds_alternative<int>(settingValue)) {
    event_value[FSTR_SETTING_value] = std::get<int>(settingValue);
  } else if (std::holds_alternative<float>(settingValue)) {
    event_value[FSTR_SETTING_value] = std::get<float>(settingValue);
  } else if (std::holds_alternative<bool>(settingValue)) {
    event_value[FSTR_SETTING_value] = std::get<bool>(settingValue);
  } else if (std::holds_alternative<String>(settingValue)) {
    event_value[FSTR_SETTING_value] = std::get<String>(settingValue);
  }

  return device->sendEvent(eventMessage);
}

template <typename T>
bool SettingController<T>::handleSettingController(SinricProRequest &request) {
  T* device = static_cast<T*>(this);

  bool success = false;

  if (setSettingCallback && request.action == FSTR_SETTING_setSetting) {
    String settingId = request.request_value[FSTR_SETTING_id] | "";
    JsonVariant valueVariant = request.request_value[FSTR_SETTING_value];

    SettingValue settingValue;

    if (valueVariant.is<bool>()) {
      settingValue = valueVariant.as<bool>();
    } else if (valueVariant.is<float>()) {
      settingValue = valueVariant.as<float>();
    } else if (valueVariant.is<int>()) {
      settingValue = valueVariant.as<int>();
    } else if (valueVariant.is<JsonObject>()) {
      String jsonString;
      serializeJson(valueVariant, jsonString);
      settingValue = jsonString;
    } else if (valueVariant.is<const char*>()) {
      settingValue = String(valueVariant.as<const char*>());
    }

    success = setSettingCallback(device->deviceId, settingId, settingValue);

    request.response_value[FSTR_SETTING_id] = settingId;

    if (valueVariant.is<JsonObject>()) {
      request.response_value[FSTR_SETTING_value] = valueVariant;
    } else if (std::holds_alternative<int>(settingValue)) {
      request.response_value[FSTR_SETTING_value] = std::get<int>(settingValue);
    } else if (std::holds_alternative<float>(settingValue)) {
      request.response_value[FSTR_SETTING_value] = std::get<float>(settingValue);
    } else if (std::holds_alternative<bool>(settingValue)) {
      request.response_value[FSTR_SETTING_value] = std::get<bool>(settingValue);
    } else if (std::holds_alternative<String>(settingValue)) {
      request.response_value[FSTR_SETTING_value] = std::get<String>(settingValue);
    }

    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

using SettingValue = SINRICPRO_NAMESPACE::SettingValue;

template <typename T>
using SettingController = SINRICPRO_NAMESPACE::SettingController<T>;