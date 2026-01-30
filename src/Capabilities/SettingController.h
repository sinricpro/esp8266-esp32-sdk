#pragma once

#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(SETTING, setSetting);     // "setSetting"
FSTR(SETTING, id);             // "id"
FSTR(SETTING, value);          // "value"

/**
 * @brief C++11-compatible variant type for setting values that can hold int, float, bool, or String
 *
 * This is a lightweight alternative to std::variant (C++17) for Arduino boards.
 */
class SettingValue {
  public:
    enum class Type { INT, FLOAT, BOOL, STRING, NONE };

    SettingValue() : type_(Type::NONE), int_val(0) {}
    SettingValue(int val) : type_(Type::INT), int_val(val) {}
    SettingValue(float val) : type_(Type::FLOAT), float_val(val) {}
    SettingValue(bool val) : type_(Type::BOOL), bool_val(val) {}
    SettingValue(const String& val) : type_(Type::STRING), string_val(val) {}
    SettingValue(const char* val) : type_(Type::STRING), string_val(val) {}

    SettingValue(const SettingValue& other) : type_(other.type_) {
      switch (type_) {
        case Type::INT:    int_val = other.int_val; break;
        case Type::FLOAT:  float_val = other.float_val; break;
        case Type::BOOL:   bool_val = other.bool_val; break;
        case Type::STRING: string_val = other.string_val; break;
        case Type::NONE:   break;
      }
    }

    SettingValue& operator=(int val) { type_ = Type::INT; int_val = val; return *this; }
    SettingValue& operator=(float val) { type_ = Type::FLOAT; float_val = val; return *this; }
    SettingValue& operator=(bool val) { type_ = Type::BOOL; bool_val = val; return *this; }
    SettingValue& operator=(const String& val) { type_ = Type::STRING; string_val = val; return *this; }
    SettingValue& operator=(const char* val) { type_ = Type::STRING; string_val = val; return *this; }

    SettingValue& operator=(const SettingValue& other) {
      if (this != &other) {
        type_ = other.type_;
        switch (type_) {
          case Type::INT:    int_val = other.int_val; break;
          case Type::FLOAT:  float_val = other.float_val; break;
          case Type::BOOL:   bool_val = other.bool_val; break;
          case Type::STRING: string_val = other.string_val; break;
          case Type::NONE:   break;
        }
      }
      return *this;
    }

    template<typename T> bool holds() const;
    template<typename T> T get() const;

    Type type() const { return type_; }

  private:
    Type type_;
    union {
      int int_val;
      float float_val;
      bool bool_val;
    };
    String string_val;  // Can't be in union
};

// Template specializations for holds()
template<> inline bool SettingValue::holds<int>() const { return type_ == Type::INT; }
template<> inline bool SettingValue::holds<float>() const { return type_ == Type::FLOAT; }
template<> inline bool SettingValue::holds<bool>() const { return type_ == Type::BOOL; }
template<> inline bool SettingValue::holds<String>() const { return type_ == Type::STRING; }

// Template specializations for get()
template<> inline int SettingValue::get<int>() const { return int_val; }
template<> inline float SettingValue::get<float>() const { return float_val; }
template<> inline bool SettingValue::get<bool>() const { return bool_val; }
template<> inline String SettingValue::get<String>() const { return string_val; }

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

  if (settingValue.holds<int>()) {
    event_value[FSTR_SETTING_value] = settingValue.get<int>();
  } else if (settingValue.holds<float>()) {
    event_value[FSTR_SETTING_value] = settingValue.get<float>();
  } else if (settingValue.holds<bool>()) {
    event_value[FSTR_SETTING_value] = settingValue.get<bool>();
  } else if (settingValue.holds<String>()) {
    event_value[FSTR_SETTING_value] = settingValue.get<String>();
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
    } else if (settingValue.holds<int>()) {
      request.response_value[FSTR_SETTING_value] = settingValue.get<int>();
    } else if (settingValue.holds<float>()) {
      request.response_value[FSTR_SETTING_value] = settingValue.get<float>();
    } else if (settingValue.holds<bool>()) {
      request.response_value[FSTR_SETTING_value] = settingValue.get<bool>();
    } else if (settingValue.holds<String>()) {
      request.response_value[FSTR_SETTING_value] = settingValue.get<String>();
    }

    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

using SettingValue = SINRICPRO_NAMESPACE::SettingValue;

template <typename T>
using SettingController = SINRICPRO_NAMESPACE::SettingController<T>;