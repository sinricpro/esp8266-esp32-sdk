#ifndef _POWERSENSORCONTROLLER_H_
#define _POWERSENSORCONTROLLER_H_

#include "./../SinricProDeviceInterface.h"

class PowerSensorController {
  public:
    PowerSensorController(SinricProDeviceInterface *device);
    bool sendPowerSensorEvent(float voltage, float current, float power = -1.0f, float apparentPower = -1.0f, float reactivePower = -1.0f, float factor = -1.0f, String cause = "PERIODIC_POLL");

  protected:
  private:
    SinricProDeviceInterface *device;
    unsigned long startTime = 0;
    unsigned long lastPower = 0;
    float getWattHours(unsigned long currentTimestamp);
};

PowerSensorController::PowerSensorController(SinricProDeviceInterface *device) : device(device) {}

/**
 * @brief Send PowerSensor event to SinricPro Server 
 * @param   voltage       voltage
 * @param   current       current
 * @param   power         (optional) if not provided, it is calculated automaticly (power = voltage * current)
 * @param   apparentPower (optional) if not provided it is set to -1
 * @param   reactivePower (optional) if not provided it is set to -1
 * @param   factor        (optional) if not provided it is set to -1 \n if apparentPower is provided, factor is calculated automaticly (factor = power / apparentPower)
 * @param   cause         (optional) Reason why event is sent (default = `"PERIODIC_POLL"`)
 * @return  the success of sending the event
 * @retval  true          event has been sent successfully
 * @retval  false         event has not been sent, maybe you sent to much events in a short distance of time
 **/
bool PowerSensorController::sendPowerSensorEvent(float voltage, float current, float power, float apparentPower, float reactivePower, float factor, String cause) {
  DynamicJsonDocument eventMessage = device->prepareEvent("powerUsage", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  if (power == -1)
    power = voltage * current;
  if (apparentPower != -1)
    factor = power / apparentPower;

  unsigned long currentTimestamp = device->getTimestamp();

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
  return device->sendEvent(eventMessage);
}

float PowerSensorController::getWattHours(unsigned long currentTimestamp) {
  if (startTime)
    return (currentTimestamp - startTime) * lastPower / 3600.0f;
  return 0;
}

#endif