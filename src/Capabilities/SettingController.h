#ifndef _SETTINGCONTROLLER_H_
#define _SETTINGCONTROLLER_H_

#include "SinricProRequest.h"

template <typename T>
class SettingController {
  public:
    SettingController() { static_cast<T &>(*this).requestHandlers.push_back(std::bind(&SettingController<T>::handleSettingController, this, std::placeholders::_1)); }
    using SetSettingCallback = std::function<bool(const String&, const String&, String&)>;
    void onSetSetting(SetSettingCallback cb);

  protected:
    bool handleSettingController(SinricProRequest &request);

  private: 
    SetSettingCallback setSettingCallback;
};

template <typename T>
void SettingController<T>::onSetSetting(SetSettingCallback cb) {
  setSettingCallback = cb;
}

template <typename T>
bool SettingController<T>::handleSettingController(SinricProRequest &request) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (setSettingCallback && request.action == "setSetting") {
    String settingId    = request.request_value["id"] | "";
    String settingValue = request.request_value["value"] | "";
    success = setSettingCallback(device.deviceId, settingId, settingValue);
    request.response_value["id"]    = settingId;
    request.response_value["value"] = settingValue;
    return success;
  }

  return success;
}

#endif