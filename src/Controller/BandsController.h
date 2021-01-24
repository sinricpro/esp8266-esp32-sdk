#ifndef _BANDSCONTROLLER_H_
#define _BANDSCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class BandsController {
  public:
    BandsController(SinricProDeviceInterface* device);

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

    void onSetBands(SetBandsCallback cb);
    void onAdjustBands(AdjustBandsCallback cb);
    void onResetBands(ResetBandsCallback cb);

    bool sendBandsEvent(String bands, int level, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value);

  private:
    SinricProDeviceInterface *device;
    SetBandsCallback setBandsCallback;
    AdjustBandsCallback adjustBandsCallback;
    ResetBandsCallback resetBandsCallback;
};

BandsController::BandsController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Set callback function for `setBands` request
 * 
 * @param cb Function pointer to a `SetBandsCallback` function
 * @return void
 * @see SetBandsCallback
 **/
void BandsController::onSetBands(SetBandsCallback cb) { setBandsCallback = cb; }

/**
 * @brief Set callback function for `adjustBands` request
 * 
 * @param cb Function pointer to a `AdjustBandsCallback` function
 * @return void
 * @see AdjustBandsCallback
 **/
void BandsController::onAdjustBands(AdjustBandsCallback cb) { adjustBandsCallback = cb; }

/**
 * @brief Set callback function for `resetBands` request
 * 
 * @param cb Function pointer to a `ResetBandsCallback` function
 * @return void
 * @see ResetBandsCallback
 **/
void BandsController::onResetBands(ResetBandsCallback cb) { resetBandsCallback = cb; }

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
bool BandsController::sendBandsEvent(String bands, int level, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("setBands", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  JsonArray event_value_bands = event_value.createNestedArray("bands");
  JsonObject event_bands = event_value_bands.createNestedObject();
  event_bands["name"] = bands;
  event_bands["value"] = level;
  return device->sendEvent(eventMessage);
}

bool BandsController::handleRequest(const char *action, JsonObject &request_value, JsonObject &response_value) {
  bool success = false;
  String actionString = String(action);

  if (setBandsCallback && actionString == "setBands") {
    JsonArray bands_array = request_value["bands"];
    JsonArray response_value_bands = response_value.createNestedArray("bands");

    for (size_t i = 0; i < bands_array.size(); i++) {
      int level = bands_array[i]["level"] | 0;
      String bandsName = bands_array[i]["name"] | "";
      success = setBandsCallback(device->getDeviceId(), bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = level;
    }
    return success;
  }

  if (adjustBandsCallback && actionString == "adjustBands") {
    JsonArray bands_array = request_value["bands"];
    JsonArray response_value_bands = response_value.createNestedArray("bands");

    for (size_t i = 0; i < bands_array.size(); i++)  {
      int levelDelta = bands_array[i]["levelDelta"] | 1;
      String direction = bands_array[i]["levelDirection"];
      if (direction == "DOWN")
        levelDelta *= -1;
      String bandsName = bands_array[i]["name"] | "";
      success = adjustBandsCallback(device->getDeviceId(), bandsName, levelDelta);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = levelDelta;
    }
    return success;
  }

  if (resetBandsCallback && actionString == "resetBands") {
    JsonArray bands_array = request_value["bands"];
    JsonArray response_value_bands = response_value.createNestedArray("bands");

    for (size_t i = 0; i < bands_array.size(); i++) {
      int level = 0;
      String bandsName = bands_array[i]["name"] | "";
      success = adjustBandsCallback(device->getDeviceId(), bandsName, level);
      JsonObject response_value_bands_i = response_value_bands.createNestedObject();
      response_value_bands_i["name"] = bandsName;
      response_value_bands_i["level"] = level;
    }
    return success;
  }

  return success;
}

#endif