#pragma once

#include "../SinricProRequest.h"
#include "../SinricProStrings.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(SETTING, setSetting);     // "setSetting"
FSTR(SETTING, id);             // "id"
FSTR(SETTING, value);          // "value"

/**
 * @brief Variant type for setting values that can hold int, float, bool, or String
 */
class SettingValue {
  public:
    enum class Type { Unknown, Int, Float, Bool, String };

    SettingValue() : type(Type::Unknown), intValue(0), floatValue(0.0f), boolValue(false) {}
    SettingValue(int value) : type(Type::Int), intValue(value), floatValue(0.0f), boolValue(false) {}
    SettingValue(float value) : type(Type::Float), intValue(0), floatValue(value), boolValue(false) {}
    SettingValue(double value) : type(Type::Float), intValue(0), floatValue(static_cast<float>(value)), boolValue(false) {}
    SettingValue(bool value) : type(Type::Bool), intValue(0), floatValue(0.0f), boolValue(value) {}
    SettingValue(const String& value) : type(Type::String), intValue(0), floatValue(0.0f), boolValue(false), stringValue(value) {}
    SettingValue(const char* value) : type(Type::String), intValue(0), floatValue(0.0f), boolValue(false), stringValue(value) {}

    Type getType() const { return type; }

    bool isInt() const { return type == Type::Int; }
    bool isFloat() const { return type == Type::Float; }
    bool isBool() const { return type == Type::Bool; }
    bool isString() const { return type == Type::String; }

    int asInt() const { return intValue; }
    float asFloat() const { return floatValue; }
    bool asBool() const { return boolValue; }
    const String& asString() const { return stringValue; }

    void setInt(int value) { type = Type::Int; intValue = value; }
    void setFloat(float value) { type = Type::Float; floatValue = value; }
    void setBool(bool value) { type = Type::Bool; boolValue = value; }
    void setString(const String& value) { type = Type::String; stringValue = value; }

  private:
    Type type;
    int intValue;
    float floatValue;
    bool boolValue;
    String stringValue;
};

using SetSettingCallback = std::function<bool(const String&, const String&, SettingValue&)>;

template <typename T>
class SettingController {
  public:
    SettingController();
    void onSetSetting(SetSettingCallback cb);

  protected:
    bool handleSettingController(SinricProRequest &request);

  private:
    SetSettingCallback setSettingCallback;
};

template <typename T>
SettingController<T>::SettingController() {
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&SettingController<T>::handleSettingController, this, std::placeholders::_1));
}

template <typename T>
void SettingController<T>::onSetSetting(SetSettingCallback cb) {
  setSettingCallback = cb;
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
      settingValue = SettingValue(valueVariant.as<bool>());
    } else if (valueVariant.is<float>()) {
      settingValue = SettingValue(valueVariant.as<float>());
    } else if (valueVariant.is<int>()) {
      settingValue = SettingValue(valueVariant.as<int>());
    } else if (valueVariant.is<const char*>()) {
      settingValue = SettingValue(valueVariant.as<const char*>());
    }

    success = setSettingCallback(device->deviceId, settingId, settingValue);

    request.response_value[FSTR_SETTING_id] = settingId;

    switch (settingValue.getType()) {
      case SettingValue::Type::Int:
        request.response_value[FSTR_SETTING_value] = settingValue.asInt();
        break;
      case SettingValue::Type::Float:
        request.response_value[FSTR_SETTING_value] = settingValue.asFloat();
        break;
      case SettingValue::Type::Bool:
        request.response_value[FSTR_SETTING_value] = settingValue.asBool();
        break;
      case SettingValue::Type::String:
        request.response_value[FSTR_SETTING_value] = settingValue.asString();
        break;
      default:
        break;
    }

    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE

using SettingValue = SINRICPRO_NAMESPACE::SettingValue;

template <typename T>
using SettingController = SINRICPRO_NAMESPACE::SettingController<T>;