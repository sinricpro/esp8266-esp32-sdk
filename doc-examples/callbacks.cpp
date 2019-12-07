/**
 * @file callbacks.cpp
 * @author sivar2311 (sivar2311@googlemail.com)
 * @brief Example callbacks
 * @version 0.1
 * @date 2019-12-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

//! [onPowerState]
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s\r\n", deviceId.c_str(), state?"on":"off");
  return true; // request handled properly
}
//! [onPowerState]

//! [onPowerLevel]
bool onPowerLevel(const String &deviceId, int &powerLevel) {
  Serial.printf("Device %s powerlevel %d\r\n", deviceId.c_str(), powerLevel);
  return true; // request handled properly
}
//! [onPowerLevel]

//! [onAdjustPowerLevel]
int absolutePowerLevel;

bool onAdjustPowerLevel(const String &deviceId, int &powerDelta) {
  absolutePowerLevel += powerDelta;  // calculate absolute power level
  Serial.printf("Device %s brightness changed about %i to %d\r\n", deviceId.c_str(), powerDelta, absolutePowerLevel);
  powerDelta = absolutePowerLevel; // return absolute power level
  return true; // request handled properly
}
//! [onAdjustPowerLevel]


//! [onBrightness]
bool onBrightness(const String &deviceId, int &brightness) {
  Serial.printf("Device %s brightness %d\r\n", deviceId.c_str(), brightness);
  return true; // request handled properly
}
//! [onBrightness]

//! [onAdjustBrightness]
int absoluteBrightness;

bool onAdjustBrightness(const String &deviceId, int &brightnessDelta) {
  absoluteBrightness += brightnessDelta; // calculate absolute brigthness
  Serial.printf("Device %s brightness changed about %i to %d\r\n", deviceId.c_str(), brightnessDelta, absoluteBrightness);
  brightnessDelta = absoluteBrightness; // return absolute brightness
  return true; // request handled properly
}
//! [onAdjustBrightness]

//! [onColor]
bool onColor(const String &deviceId, byte &r, byte &g, byte &b) {
  Serial.printf("Device %s color is set to red=%d, green=%d, blue=%d\r\n", deviceId.c_str(), r, g, b);
  return true; // request handled properly
}
//! [onColor]

//! [onColorTemperature]
bool onColorTemperature(const String &deviceId, int &colorTemperature) {
  Serial.printf("Device %s colorTemperature is set to %i\r\n", deviceId.c_str(), colorTemperature);
  return true; // request handled properly
}
//! [onColorTemperature]

//! [onIncreaseColorTemperature]
int globalColorTemperature; 

bool onIncreaseColorTemperature(const String &deviceId, int &colorTemperature) {
  globalColorTemperature += 1000; // increase globalColorTemperature about 1000;
  Serial.printf("Device %s colorTemperature changed to %i\r\n", deviceId.c_str(), colorTemperature);
  colorTemperature = globalColorTemperature; // return new colorTemperature
  return true; // request handled properly
}
//! [onIncreaseColorTemperature]

//! [onDecreaseColorTemperature]
int globalColorTemperature; 

bool onDecreaseColorTemperature(const String &deviceId, int &colorTemperature) {
  globalColorTemperature -= 1000; // decrease globalColorTemperature about 1000;
  Serial.printf("Device %s colorTemperature changed %i\r\n", deviceId.c_str(), colorTemperature);
  colorTemperature = globalColorTemperature; // return new colorTemperature
  return true; // request handled properly
}
//! [onDecreaseColorTemperature]

//! [onRangeValue]
bool onRangeValue(const String &deviceId, int &rangeValue) {
  Serial.printf("Device %s range value has been set to %d\r\n", rangeValue);
  return true; // request handled properly
}
//! [onRangeValue]

//! [onAdjustRangeValue]
int globalRangeValue;

bool onAdjustRangeValue(const String &deviceId, int &rangeValueDelta) {
  globalRangeValue += rangeValue; // calculate absolute rangeValue
  Serial.printf("Device %s range value has been changed about %i to %d\r\n", rangeValueDelta, globalRangeValue);
  rangeValueDelta = globalRangeValue; // return absolute rangeValue
  return true; // request handled properly
}
//! [onAdjustRangeValue]

//! [onTargetTemperature]
bool onTargetTemperature(const String &deviceId, float &targetTemp) { 
  Serial.printf("Device %s target temperature set to %f\r\n", deviceId.c_str(), targetTemp);
  return true; // request handled properly
}
//! [onTargetTemperature]

//! [onAdjustTargetTemperature]
float globalTargetTemp; 

bool onAdjustTargetTemperature(const String &deviceId, float &tempDelta) {
  globalTargetTemp += tempDelta; // change global target temperature about tempDelta
  Serial.printf("Device %s target temperature changed about %f to %f\r\n", deviceId.c_str(), tempDelta, globalTargetTemp);
  tempDelta = globalTargetTemp; // return absolute target temperature
  return true; // request handled properly
}
//! [onAdjustTargetTemperature]

//! [onThermostatMode]
bool onThermostatMode(const String& deviceId, String& mode) {
  Serial.printf("Deivce %s thermostat mode set to %s\r\n", deviceId.c_str(), mode);
  return true; // request handled properly
}
//! [onThermostatMode]
