/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProRequest.h"
#include "SinricProStrings.h"
#include "SinricProNamespace.h"
#include "SinricProDebug.h"

namespace SINRICPRO_NAMESPACE {

FSTR(OTA, otaUpdateAvailable);  // "otaUpdateAvailable"
FSTR(OTA, url);                 // "url"
FSTR(OTA, version);             // "version"
FSTR(OTA, major);               // "major"
FSTR(OTA, minor);               // "minor"
FSTR(OTA, patch);               // "patch"
FSTR(OTA, forceUpdate);         // "forceUpdate"
FSTR(SETTINGS, setSetting);     // "setSetting"
FSTR(SETTINGS, id);             // "id"
FSTR(SETTINGS, value);          // "value"
FSTR(INSIGHTS, health);         // "health"
FSTR(INSIGHTS, report);         // "report"

using OTAUpdateCallbackHandler = std::function<bool(const String& url, int major, int minor, int patch, bool forceUpdate)>;
using SetSettingCallbackHandler = std::function<bool(const String& id, const String& value)>;
using ReportHealthCallbackHandler = std::function<bool(String& healthReport)>;

class SinricProModuleCommandHandler {
  public:
    SinricProModuleCommandHandler();
    ~SinricProModuleCommandHandler();

    bool handleRequest(SinricProRequest &request);
    void onOTAUpdate(OTAUpdateCallbackHandler callback);
    void onSetSetting(SetSettingCallbackHandler callback);
    void onReportHealth(ReportHealthCallbackHandler callback);

  private:
    OTAUpdateCallbackHandler _otaUpdateCallbackHandler;
    SetSettingCallbackHandler _setSettingCallbackHandler;
    ReportHealthCallbackHandler _reportHealthCallbackHandler;
};

SinricProModuleCommandHandler::SinricProModuleCommandHandler()
    : _otaUpdateCallbackHandler(nullptr),
     _setSettingCallbackHandler(nullptr),
     _reportHealthCallbackHandler(nullptr) {}

SinricProModuleCommandHandler::~SinricProModuleCommandHandler() {}

void SinricProModuleCommandHandler::onOTAUpdate(OTAUpdateCallbackHandler callback) {
  _otaUpdateCallbackHandler = callback;
}

void SinricProModuleCommandHandler::onSetSetting(SetSettingCallbackHandler callback) {
  _setSettingCallbackHandler = callback;
}

void SinricProModuleCommandHandler::onReportHealth(ReportHealthCallbackHandler callback) {
  _reportHealthCallbackHandler = callback;
}

bool SinricProModuleCommandHandler::handleRequest(SinricProRequest &request) {
  if (strcmp(FSTR_OTA_otaUpdateAvailable, request.action.c_str()) == 0 && _otaUpdateCallbackHandler) {
    String url = request.request_value[FSTR_OTA_url];        
    int major  = request.request_value[FSTR_OTA_version][FSTR_OTA_major]; 
    int minor  = request.request_value[FSTR_OTA_version][FSTR_OTA_minor]; 
    int patch  = request.request_value[FSTR_OTA_version][FSTR_OTA_patch]; 
    bool forceUpdate  = request.request_value[FSTR_OTA_version][FSTR_OTA_forceUpdate] | false; 
    return _otaUpdateCallbackHandler(url, major, minor, patch, forceUpdate);
  }
  else if (strcmp(FSTR_SETTINGS_setSetting, request.action.c_str()) == 0 && _setSettingCallbackHandler) {    
    String id = request.request_value[FSTR_SETTINGS_id];
    String value = request.request_value[FSTR_SETTINGS_value];
    return _setSettingCallbackHandler(id, value);
  } 
  else if (strcmp(FSTR_INSIGHTS_health, request.action.c_str()) == 0 && _reportHealthCallbackHandler) {    
    String healthReport = "";
    bool success = _reportHealthCallbackHandler(healthReport);
    if (success) {
      request.response_value[FSTR_INSIGHTS_report] = healthReport;
    }
    return success;
  }
  else {
     DEBUG_SINRIC("[SinricProModuleCommandHandler:handleRequest]: action: %s not supported!\r\n", request.action.c_str());
  }
  return false;
}

} // SINRICPRO_NAMESPACE
