/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRICAIRQUALITYSENSOR_H_
#define _SINRICAIRQUALITYSENSOR_H_

#include "SinricProDevice.h"

/**
 * @class SinricProAirQualitySensor
 * @brief Device to report air quality events
 */
class SinricProAirQualitySensor :  public SinricProDevice {
  public:
	  SinricProAirQualitySensor(const char* deviceId, unsigned long eventWaitTime=100);
    String getProductType() { return SinricProDevice::getProductType() + String("AIR_QUALITY_SENSOR"); }

    // event
    bool sendAirQualityEvent(int pm1=0, int pm2_5=0, int pm10=0, String cause = "PERIODIC_POLL");
  private:
};

SinricProAirQualitySensor::SinricProAirQualitySensor(const char* deviceId, unsigned long eventWaitTime) : SinricProDevice(deviceId, eventWaitTime) {}

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
bool SinricProAirQualitySensor::sendAirQualityEvent(int pm1, int pm2_5, int pm10, String cause) {
  DynamicJsonDocument eventMessage = prepareEvent(deviceId, "airQuality", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];

  event_value["pm1"] = limitValue(pm1, 0, 999);
  event_value["pm2_5"] = limitValue(pm2_5, 0, 999);
  event_value["pm10"] = limitValue(pm10, 0, 999);
  
  return sendEvent(eventMessage);
}

#endif

