#ifndef _POWERSENSOR_H_
#define _POWERSENSOR_H_

/**
 * @brief PowerSensor
 * @ingroup Capabilities
 **/
template <typename T>
class PowerSensor {
public:
  bool sendPowerSensorEvent(float voltage, float current, float power = -1.0f, float apparentPower = -1.0f, float reactivePower = -1.0f, float factor = -1.0f, String cause = "PERIODIC_POLL");

private:
  unsigned long startTime = 0;
  unsigned long lastPower = 0;
  float getWattHours(unsigned long currentTimestamp);
};

/**
 * @brief Send PowerSensor event to SinricPro Server 
 * @param   voltage       `float` voltage
 * @param   current       `float` current
 * @param   power         `float` (optional) if not provided, it is calculated automaticly (power = voltage * current)
 * @param   apparentPower `float` (optional) if not provided it is set to -1
 * @param   reactivePower `float` (optional) if not provided it is set to -1
 * @param   factor        `float` (optional) if not provided it is set to -1 \n if apparentPower is provided, factor is calculated automaticly (factor = power / apparentPower)
 * @param   cause         `String` (optional) Reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool PowerSensor<T>::sendPowerSensorEvent(float voltage, float current, float power, float apparentPower, float reactivePower, float factor, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("powerUsage", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  if (power == -1)
    power = voltage * current;
  if (apparentPower != -1)
    factor = power / apparentPower;

  unsigned long currentTimestamp = device.getTimestamp();

  event_value["startTime"] = startTime;
  event_value["voltage"] = voltage;
  event_value["current"] = current;
  event_value["power"] = power;
  event_value["apparentPower"] = apparentPower;
  event_value["reactivePower"] = reactivePower;
  event_value["factor"] = factor;
  event_value["wattHours"] = getWattHours(currentTimestamp);

  startTime = currentTimestamp;
  lastPower = power;
  return device.sendEvent(eventMessage);
}

template <typename T>
float PowerSensor<T>::getWattHours(unsigned long currentTimestamp) {
  if (startTime)
    return (currentTimestamp - startTime) * lastPower / 3600.0f;
  return 0;
}

#endif