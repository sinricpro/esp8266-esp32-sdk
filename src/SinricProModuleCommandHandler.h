/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProRequest.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

FSTR(OTA, otaUpdateAvailable);  // "otaUpdateAvailable"
FSTR(OTA, url);                 // "url"
FSTR(OTA, version);             // "version"
FSTR(OTA, major);               // "major"
FSTR(OTA, minor);               // "minor"
FSTR(OTA, patch);               // "patch"
FSTR(SETTINGS, setSetting);     // "setSetting"
FSTR(SETTINGS, getSetting);     // "getSetting"
FSTR(SETTINGS, id);             // "id"
FSTR(SETTINGS, value);          // "value"

using OTAUpdateCallbackHandler = std::function<bool(const String& url, int major, int minor, int patch)>;
using SetSettingCallbackHandler = std::function<bool(const String& id, const String& value)>;

class SinricProModuleCommandHandler {
  public:
    SinricProModuleCommandHandler();
    ~SinricProModuleCommandHandler();

    bool handleRequest(SinricProRequest &request);
    void onOTAUpdate(OTAUpdateCallbackHandler callback);
    void onSetSetting(SetSettingCallbackHandler callback);

  private:
    OTAUpdateCallbackHandler  _otaUpdateCallbackHandler;
    SetSettingCallbackHandler _setSettingCallbackHandler;
};

SinricProModuleCommandHandler::SinricProModuleCommandHandler()
    : _otaUpdateCallbackHandler(nullptr),
     _setSettingCallbackHandler(nullptr) {}

SinricProModuleCommandHandler::~SinricProModuleCommandHandler() {}

void SinricProModuleCommandHandler::onOTAUpdate(OTAUpdateCallbackHandler callback) {
  _otaUpdateCallbackHandler = callback;
}

void SinricProModuleCommandHandler::onSetSetting(SetSettingCallbackHandler callback) {
  _setSettingCallbackHandler = callback;
}
 
bool SinricProModuleCommandHandler::handleRequest(SinricProRequest &request) {
  if (strcmp(FSTR_OTA_otaUpdateAvailable, request.action.c_str()) == 0 && _otaUpdateCallbackHandler) {
    String url = request.request_value[FSTR_OTA_url];        
    int major  = request.request_value[FSTR_OTA_version][FSTR_OTA_major]; 
    int minor  = request.request_value[FSTR_OTA_version][FSTR_OTA_minor]; 
    int patch  = request.request_value[FSTR_OTA_version][FSTR_OTA_patch]; 
    return _otaUpdateCallbackHandler(url, major, minor, patch);
  }
  else if (strcmp(FSTR_SETTINGS_setSetting, request.action.c_str()) == 0 && _setSettingCallbackHandler) {    
    String id = request.request_value[FSTR_SETTINGS_id];
    String value = request.request_value[FSTR_SETTINGS_value];
    return _setSettingCallbackHandler(id, value);
  }
  return false;
}

} // SINRICPRO_NAMESPACE
