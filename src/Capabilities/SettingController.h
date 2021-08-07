#pragma once

#include "../SinricProNamespace.h"
#include "../SinricProRequest.h"
#include "../SinricProStrings.h"
namespace SINRICPRO_NAMESPACE {

using SetSettingCallback = std::function<bool(const String &, const String &, String &)>;

FSTR(SETTING, setSetting);  // "setSetting"
FSTR(SETTING, id);          // "id"
FSTR(SETTING, value);       // "value"

template <typename T>
class SettingController {
  public:
    SettingController();
    void onSetSetting(SetSettingCallback cb);

  protected:
    virtual bool onSetSetting(const String &settingId, String &settingValue);
    bool handleSettingController(SinricProRequest &request);

  private:
    SetSettingCallback setSettingCallback;
};

template <typename T>
SettingController<T>::SettingController() {
    T *device = static_cast<T *>(this);
    device->registerRequestHandler(std::bind(&SettingController<T>::handleSettingController, this, std::placeholders::_1));
}

template <typename T>
void SettingController<T>::onSetSetting(SetSettingCallback cb) {
    setSettingCallback = cb;
}

template <typename T>
bool SettingController<T>::onSetSetting(const String &settingId, String &settingValue) {
    T *device = static_cast<T *>(this);
    if (setSettingCallback) return setSettingCallback(device->deviceId, settingId, settingValue);
    return false;
}

template <typename T>
bool SettingController<T>::handleSettingController(SinricProRequest &request) {
    bool success = false;

    if (request.action == FSTR_SETTING_setSetting) {
        String settingId = request.request_value[FSTR_SETTING_id] | "";
        String settingValue = request.request_value[FSTR_SETTING_value] | "";
        success = onSetSetting(settingId, settingValue);
        request.response_value[FSTR_SETTING_id] = settingId;
        request.response_value[FSTR_SETTING_value] = settingValue;
        return success;
    }

    return success;
}

}  // namespace SINRICPRO_NAMESPACE