#ifndef _TEMPERATURECONTROLLER_H_
#define _TEMPERATURECONTROLLER_H_

/**
 * @brief TemperatureEventSource
 * @ingroup EventSource
 **/
template <typename T>
class TemperatureEventSource {
  public:
    bool sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL");
};

/**
 * @brief Send `currentTemperature` event to report actual temperature (measured by a sensor)
 * 
 * @param   temperature   Float with actual temperature measured by a sensor
 * @param   humidity      (optional) Float with actual humidity measured by a sensor (default=-1.0f means not supported)
 * @param   cause         (optional) `String` reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the even
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool TemperatureEventSource<T>::sendTemperatureEvent(float temperature, float humidity, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("currentTemperature", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["humidity"] = humidity;
  event_value["temperature"] = roundf(temperature * 10) / 10;
  return device.sendEvent(eventMessage);
}

#endif