#ifndef _AIRQUALITYCONTROLLER_H_
#define _AIRQUALITYCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class AirQualityController {
  public:
    AirQualityController(SinricProDeviceInterface* device);

    bool sendAirQualityEvent(int pm1 = 0, int pm2_5 = 0, int pm10 = 0, String cause = "PERIODIC_POLL");

  private:
    SinricProDeviceInterface* device;
};

AirQualityController::AirQualityController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Sending air quality to SinricPro server
 * 
 * @param   pm1           1.0 μm particle pollutant	in μg/m3
 * @param   pm2_5         2.5 μm particle pollutant	in μg/m3
 * @param   pm10          10 μm particle pollutant in μg/m3
 * @param   cause         (optional) `String` reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool AirQualityController::sendAirQualityEvent(int pm1, int pm2_5, int pm10, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("airQuality", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];

  event_value["pm1"] = pm1;
  event_value["pm2_5"] = pm2_5;
  event_value["pm10"] = pm10;

  return device->sendEvent(eventMessage);
}

#endif