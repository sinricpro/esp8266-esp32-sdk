#ifndef _AIRQUALITYSENSOR_H_
#define _AIRQUALITYSENSOR_H_

/**
 * @brief AirQuality
 * @ingroup Capabilities
 **/
template <typename T>
class AirQualitySensor {
  public:
    bool sendAirQualityEvent(int pm1 = 0, int pm2_5 = 0, int pm10 = 0, String cause = "PERIODIC_POLL");
};

/**
 * @brief Sending air quality to SinricPro server
 * 
 * @param   pm1           `int` 1.0 μm particle pollutant	in μg/m3
 * @param   pm2_5         `int` 2.5 μm particle pollutant	in μg/m3
 * @param   pm10          `int` 10 μm particle pollutant in μg/m3
 * @param   cause         (optional) `String` reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool AirQualitySensor<T>::sendAirQualityEvent(int pm1, int pm2_5, int pm10, String cause) {
  T& device = static_cast<T&>(*this);
  
  DynamicJsonDocument eventMessage = device.prepareEvent("airQuality", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];

  event_value["pm1"] = pm1;
  event_value["pm2_5"] = pm2_5;
  event_value["pm10"] = pm10;

  return device.sendEvent(eventMessage);
}

#endif