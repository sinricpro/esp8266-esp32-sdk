#include "credentials.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>

#include "SinricPro.h"

#include "SinricProSwitch.h"
#include "SinricProDimSwitch.h"
#include "SinricProLight.h"
#include "SinricProDoorbell.h"
#include "SinricProTemperaturesensor.h"
#include "SinricProThermostat.h"
#include "SinricProLock.h"
#include "SinricProMotionsensor.h"
#include "SinricProContactsensor.h"
#include "SinricProTV.h"
#include "SinricProSpeaker.h"
#include "SinricProFan.h"
#include "SinricProFanUS.h"

bool myPowerState = false;
int myPowerLevel = 0;
int myBrightness = 0;
int myColorTemp = 2200;
int myVolume = 0;

bool onPowerState(String deviceId, bool &state) {
  Serial.printf("onPowerStateString(%s, %s)\r\n", deviceId.c_str(), state?"On":"Off");
  return true;
}

bool onPowerLevel(String deviceId, int& powerLevel) {
  Serial.printf("onPowerLevel(%s, %d)\r\n", deviceId.c_str(), powerLevel);
  myPowerLevel = powerLevel;
  return true;
}

bool onAdjustPowerLevel(String deviceId, int& powerLevelDelta) {
  myPowerLevel += powerLevelDelta;
  powerLevelDelta = myPowerLevel;
  Serial.printf("onAdjustPowerLevel(%s, %d)\r\n", deviceId.c_str(), powerLevelDelta);
  return true;
}

bool onTargetTemperature(String deviceId, float temperature) {
  Serial.printf("onTargetTemperature(%s, %f)\r\n", deviceId.c_str(), temperature);
  return true;
}

bool onThermostatMode(String deviceId, String mode) {
  Serial.printf("onThermostatMode(%s, %s)\r\n", deviceId.c_str(), mode.c_str());
  return true;
}

bool onLockState(String deviceId, bool& lockState) {
  Serial.printf("onLockState(%s, %s)\r\n", deviceId.c_str(), lockState?"lock":"unlock");
  return true;
}

bool onBrightness(String deviceId, int& brightness) {
  Serial.printf("onBrightness(%s, %d)\r\n", deviceId.c_str(), brightness);
  return true;
}

bool onAdjustBrightness(String deviceId, int& brightnessDelta) {
  myBrightness += brightnessDelta;
  brightnessDelta = myBrightness;
  Serial.printf("onAdjustBrightness(%s, %d)\r\n", deviceId.c_str(), brightnessDelta);
  return true;
}

bool onColor(String deviceId, byte& r, byte& g, byte& b) {
  Serial.printf("onColor(%s, %d,%d,%d)\r\n", deviceId.c_str(), r, g, b);
  return true;
}

bool onColorTemperature(String deviceId, int& colorTemperature) {
  myColorTemp = colorTemperature;
  Serial.printf("onColorTemperature(%s, %d)\r\n", deviceId.c_str(), colorTemperature);
  return true;
}

bool onIncreaseColorTemperature(String deviceId, int& colorTemperature) {
  myColorTemp += 1000;
  colorTemperature = myColorTemp;
  Serial.printf("onIncreaseColorTemperature(%s, %d)\r\n", deviceId.c_str(), colorTemperature);
  return true;
}

bool onDecreaseColorTemperature(String deviceId, int& colorTemperature) {
  myColorTemp -= 1000;
  colorTemperature = myColorTemp;
  Serial.printf("onDecreaseColorTemperature(%s, %d)\r\n", deviceId.c_str(), colorTemperature);
  return true;
}

bool onChangeChannel(String deviceId, String& channel) {
  Serial.printf("onChangeChannel(%s, %s)\r\n", deviceId.c_str(), channel.c_str());
  return true;
}

bool onSelectInput(String deviceId, String& input) {
  Serial.printf("onSelectInput(%s, %s)\r\n", deviceId.c_str(), input.c_str());
  return true;
}

bool onSetVolume(String deviceId, int& volume) { 
  Serial.printf("onSetVolume(%s, %d)\r\n", deviceId.c_str(), volume);
  return true;
}

bool onAdjustVolume(String deviceId, int& volumeDelta) {
  Serial.printf("onAdjustVolume(%s, %d)\r\n", deviceId.c_str(), volumeDelta);
  myVolume += volumeDelta;
  volumeDelta = myVolume;
  return true;
}

bool onMute(String deviceId, bool& mute) {
  Serial.printf("onMute(%s, %s)\r\n", deviceId.c_str(), mute?"mute":"unmute");
  return true;
} 

bool onMediaControl(String deviceId, String& control) {
  Serial.printf("onMute(%s, %s)\r\n", deviceId.c_str(), control.c_str());
  return true;
}

bool onSkipChannels(String deviceId, int channelCount, String& channelName) {
  Serial.printf("onSkipChannels(%s, %d)\r\n", deviceId.c_str(), channelCount);
  channelName = "HAB ICH NICHT!";
  return true;
}

bool onSetBands(String deviceId, String band, int& level) {
  Serial.printf("onSetBands(%s, %s, %d)\r\n", deviceId.c_str(), band.c_str(), level);
  return true;
}

bool onAdjustBands(String deviceId, String band, int& levelDelta) {
  Serial.printf("onAdjustBands(%s, %s, %d)\r\n", deviceId.c_str(), band.c_str(), levelDelta);
  return true;
}

bool onResetBands(String deviceId, String band, int& level) {
  Serial.printf("onResetBands(%s, %s)\r\n", deviceId.c_str(), band.c_str());
  level = 0;
  return true;
}

bool onSetMode(String deviceId, String& mode) {
  Serial.printf("onSetMode(%s, %s)\r\n", deviceId.c_str(), mode.c_str());
  return true;
}

bool onRangeValue(String deviceId, int& rangeValue) {
  Serial.printf("onRangeValue(%s, %d)\r\n", deviceId.c_str(), rangeValue);
  return true;
}

SinricProSwitch mySwitch(SWITCH_ID, 10000);
SinricProDimSwitch myDimSwitch(DIMSWITCH_ID, 10000);
SinricProLight myLight(LIGHT_ID,10000);
SinricProDoorbell myDoorbell(DOORBELL_ID, 10000);
SinricProTemperaturesensor myTempSensor(TEMPSENSOR_ID);
SinricProThermostat myThermostat(THERMOSTAT_ID, 30000);
SinricProLock myLock(LOCK_ID, 1000);
SinricProMotionsensor myMotionsensor(MOTION_SENSOR_ID, 1000);
SinricProContactsensor myContactsensor(CONTACT_SENSOR_ID, 1000);
SinricProTV myTV(TV_ID);
SinricProSpeaker mySpeaker(SPEAKER_ID);
SinricProFan myFan(FAN_ID);
SinricProFanUS myFanUS(FAN_US);

void setupWiFi() {
  WiFi.hostname("SinricESP");
  ArduinoOTA.setHostname("SinricESP");
  
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupOTA() {
  ArduinoOTA.onStart([]() { SinricPro.stop(); Serial.printf("Start updating %s\r\n", ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "spiffs"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void setupSinricPro() {
  mySwitch.onPowerState(onPowerState);

  myDimSwitch.onPowerState(onPowerState);
  myDimSwitch.onPowerLevel(onPowerLevel);
  myDimSwitch.onAdjustPowerLevel(onAdjustPowerLevel);

  myLight.onPowerState(onPowerState);
  myLight.onBrightness(onBrightness);
  myLight.onAdjustBrightness(onAdjustBrightness);
  myLight.onColor(onColor);
  myLight.onColorTemperature(onColorTemperature);
  myLight.onIncreaseColorTemperature(onIncreaseColorTemperature);
  myLight.onDecreaseColorTemperature(onDecreaseColorTemperature);

  myDoorbell.onPowerState(onPowerState);
  
  myTempSensor.onPowerState(onPowerState);

  myThermostat.onPowerState(onPowerState);
  myThermostat.onTargetTemperatue(onTargetTemperature);
  myThermostat.onThermostatMode(onThermostatMode);

  myLock.onLockState(onLockState);

  myMotionsensor.onPowerState(onPowerState);

  myContactsensor.onPowerState(onPowerState);

  myTV.onPowerState(onPowerState);
  myTV.onChangeChannel(onChangeChannel);
  myTV.onSelectInput(onSelectInput);
  myTV.onSetVolume(onSetVolume);
  myTV.onAdjustVolume(onAdjustVolume);
  myTV.onMediaControl(onMediaControl);
  myTV.onSkipChannels(onSkipChannels);

  mySpeaker.onPowerState(onPowerState);
  mySpeaker.onSetVolume(onSetVolume);
  mySpeaker.onAdjustVolume(onAdjustVolume);
  mySpeaker.onMute(onMute);
  mySpeaker.onMediaControl(onMediaControl);
  mySpeaker.onSetBands(onSetBands);
  mySpeaker.onAdjustBands(onAdjustBands);
  mySpeaker.onResetBands(onResetBands);
  mySpeaker.onSetMode(onSetMode);

  myFan.onPowerState(onPowerState);
  myFan.onPowerLevel(onPowerLevel);

  myFanUS.onPowerState(onPowerState);
  myFanUS.onRangeValue(onRangeValue);

  SinricPro.add(mySwitch);
  SinricPro.add(myDimSwitch);
  SinricPro.add(myLight);
  SinricPro.add(myDoorbell);
  SinricPro.add(myTempSensor);
  SinricPro.add(myThermostat);
  SinricPro.add(myLock);
  SinricPro.add(myMotionsensor);
  SinricPro.add(myContactsensor);
  SinricPro.add(myTV);
  SinricPro.add(mySpeaker);
  SinricPro.add(myFan);
  SinricPro.add(myFanUS);

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  pinMode(0, INPUT_PULLUP);
  Serial.begin(115200);
  setupWiFi();
  setupOTA();
  setupSinricPro();
}

bool contact;
bool oldState;

void loop() {
  ArduinoOTA.handle();
  SinricPro.handle();
}
