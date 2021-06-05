#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using SetBandsCallback    = std::function<bool(const String &, const String &, int &)>;
using AdjustBandsCallback = std::function<bool(const String &, const String &, int &)>;
using ResetBandsCallback  = std::function<bool(const String &, const String &, int &)>;

template <typename T>
class EqualizerController {
  public:
    EqualizerController();

    void onSetBands(SetBandsCallback cb);
    void onAdjustBands(AdjustBandsCallback cb);
    void onResetBands(ResetBandsCallback cb);

    bool sendBandsEvent(String bands, int level, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onSetBands(const String& bands, int &level);
    virtual bool onAdjustBands(const String &bands, int &levelDelta);
    virtual bool onResetBands(const String &bands, int &level);

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


template <typename T>
void EqualizerController<T>::onSetBands(SetBandsCallback cb) {
  setBandsCallback = cb;
}

template <typename T>
void EqualizerController<T>::onAdjustBands(AdjustBandsCallback cb) {
  adjustBandsCallback = cb;
}

template <typename T>
void EqualizerController<T>::onResetBands(ResetBandsCallback cb) {
  resetBandsCallback = cb;
}

template <typename T>
bool EqualizerController<T>::onSetBands(const String& bands, int &level) {
  T* device = static_cast<T*>(this);
  if (setBandsCallback) return setBandsCallback(device->deviceId, bands, level);
  return false;
 
}

template <typename T>
bool EqualizerController<T>::onAdjustBands(const String &bands, int &levelDelta) {
  T* device = static_cast<T*>(this);
  if (adjustBandsCallback) return adjustBandsCallback(device->deviceId, bands, levelDelta);
  return false;
}

template <typename T>
bool EqualizerController<T>::onResetBands(const String &bands, int &level) {
  T* device = static_cast<T*>(this);
  if (resetBandsCallback) return resetBandsCallback(device->deviceId, bands, level);
  return false;
}

template <typename T>
bool EqualizerController<T>::sendBandsEvent(String bands, int level, String cause) {
  if (event_limiter) return false;
  T* device = static_cast<T*>(this);

  DynamicJsonDocument eventMessage = device->prepareEvent("setBands", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  JsonArray event_value_bands = event_value.createNestedArray("bands");
  JsonObject event_bands = event_value_bands.createNestedObject();
  event_bands["name"] = bands;
  event_bands["value"] = level;
  return device->sendEvent(eventMessage);
}

template <typename T>
bool EqualizerController<T>::handleEqualizerController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setBands") {
    JsonArray bands_array = request.request_value["bands"];
    JsonArray response_value_bands = request.response_value.createNestedArray("bands");

    for (size_t i = 0; i < bands_array.size(); i++) {
      int level = bands_array[i]["level"] | 0;
      String bandsName = bands_array[i]["name"] | "";
      success = onSetBands(bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = level;
    }
    return success;
  }

  if (request.action == "adjustBands") {
    JsonArray bands_array = request.request_value["bands"];
    JsonArray response_value_bands = request.response_value.createNestedArray("bands");

    for (size_t i = 0; i < bands_array.size(); i++)  {
      int levelDelta = bands_array[i]["levelDelta"] | 1;
      String direction = bands_array[i]["levelDirection"];
      if (direction == "DOWN") levelDelta *= -1;
      String bandsName = bands_array[i]["name"] | "";
      success = onAdjustBands(bandsName, levelDelta);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = levelDelta;
    }
    return success;
  }

  if (request.action == "resetBands") {
    JsonArray bands_array = request.request_value["bands"];
    JsonArray response_value_bands = request.response_value.createNestedArray("bands");

    for (size_t i = 0; i < bands_array.size(); i++) {
      int level = 0;
      String bandsName = bands_array[i]["name"] | "";
      success = onResetBands(bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = level;
    }
    return success;
  }

  return success;
}

} // SINRICPRO_NAMESPACE