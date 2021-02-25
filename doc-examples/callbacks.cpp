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

//! [onToggleState]
bool onToggleState(const String &deviceId, const String &instance, bool &state) {
  Serial.printf("Device %s state \"%s\" turned %s\r\n", deviceId.c_str(), instance.c_str(), state ? "on" : "off");
  return true; // request handled properly
}
//! [onToggleState]

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
  Serial.printf("Device %s range value has been set to %d\r\n", deviceId.c_str(), rangeValue);
  return true; // request handled properly
}
//! [onRangeValue]

//! [onRangeValueGeneric]
bool onRangeValue(const String &deviceId, const String &instance, int &rangeValue) {
  Serial.printf("Device %s range value for %s has been set to %d\r\n", deviceId.c_str(), instance.c_str(), rangeValue);
  return true; // request handled properly
}
//! [onRangeValueGeneric]

//! [onAdjustRangeValue]
int globalRangeValue;

bool onAdjustRangeValue(const String &deviceId, int &rangeValueDelta) {
  globalRangeValue += rangeValue; // calculate absolute rangeValue
  Serial.printf("Device %s range value has been changed about %i to %d\r\n", deviceId.c_str(), rangeValueDelta, globalRangeValue);
  rangeValueDelta = globalRangeValue; // return absolute rangeValue
  return true; // request handled properly
}
//! [onAdjustRangeValue]

//! [onAdjustRangeValueGeneric]
int globalRangeValue;

bool onAdjustRangeValue(const String &deviceId, const String& instance, int &rangeValueDelta) {
  globalRangeValue += rangeValueDelta; // calculate absolute rangeValue
  Serial.printf("Device %s range value for %s has been changed about %i to %d\r\n", deviceId.c_str(), instance.c_str(), rangeValueDelta, globalRangeValue);
  rangeValueDelta = globalRangeValue; // return absolute rangeValue
  return true;                        // request handled properly
}
//! [onAdjustRangeValueGeneric]

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


//! [onSetVolume]
bool onSetVolume(const String &deviceId, int &volume) {
  Serial.printf("Device %s volume set to %d\r\n", deviceId.c_str(), volume);
  return true; // request handled properly
}
//! [onSetVolume]

//! [onAdjustVolume]
int absoluteVolume;

bool onAdjustVolume(const String &deviceId, int &volumeDelta, bool volumeDefault) {
  absoluteVolume += volumeDelta;  // calculate absolute volume
  Serial.printf("Device %s volume changed about %i to %d (default: %s)\r\n", deviceId.c_str(), volumeDelta, absoluteVolume, volumeDefault);
  volumeDelta = absoluteVolume; // return absolute volume
  return true; // request handled properly
}
//! [onAdjustVolume]

//! [onMute]
bool onMute(const String &deviceId, bool &mute) {
  Serial.printf("Device %s is %s\r\n", deviceId.c_str(), mute?"muted":"unmuted");
  return true; // request handled properly
}
//! [onMute]

//! [onMediaControl]
bool onMediaControl(const String &deviceId, String &control) {
  Serial.printf("Device %s: %s\r\n", deviceId.c_str(), control.c_str());
  return true; // request handled properly
}
//! [onMediaControl]

//! [onSetBands]
std::map<String, int> equalizerBands;

bool onSetBands(const String &deviceId, String &bands, int &level) {
  Serial.printf("Device %s bands %s set to %d\r\n", deviceId.c_str(), bands.c_str(), level);
  equalizerBands[bands] = level; 
  return true; // request handled properly
}
//! [onSetBands]

//! [onAdjustBands]
std::map<String, int> equalizerBands;

bool onAdjustBands(const String &deviceId, String &bands, int &levelDelta) {
  equalizerBands[bands] += levelDelta; // calculate absolute bands level
  Serial.printf("Device %s bands %s changed about %d to %d\r\n", deviceId.c_str(), bands.c_str(), levelDelta, equalizerBands[bands]);
  levelDelta = equalizerBands[bands]; // return absolute bands level
  return true; // request handled properly
}
//! [onAdjustBands]


//! [onResetBands]
std::map<String, int> equalizerBands;

bool onResetBands(const String &deviceId, String &bands, int &level) {
  equalizerBands[bands] = 0; // reset bands level to 0
  Serial.printf("Device %s bands %s reset to %d\r\n", deviceId.c_str(), bands.c_str(), equalizerBands[bands]);
  return true; // request handled properly
}
//! [onResetBands]


//! [onSetMode]
bool onSetMode(const String &deviceId, String &mode) {
  Serial.printf("Device %s is set to mode %s\r\n", deviceId.c_str(), mode);
  return true; // request handled properly
}
//! [onSetMode]

//! [onSetModeGeneric] 
bool onSetMode(const String &deviceId, const String &instance, String &mode) {
  Serial.printf("Device %s mode %s set to %s\r\n", deviceId.c_str(), instance.c_str(), mode);
  return true; // request handled properly
}
//! [onSetModeGeneric]

//! [onChangeChannel]
// channelNames used to convert channelNumber into channelName
// please put in your TV channel names
// channel numbers starts counting from 0!
// so "ZDF" is channel 2
const char* channelNames[] = {
  "A/V",
  "ard", 
  "ZDF", 
  "n. d. r.", 
  "kabel eins", 
  "VOX", 
  "Sat.1", 
  "ProSieben", 
  "rtl", 
  "RTL II", 
  "SUPER RTL", 
  "KiKA"
};

int tvChannel; // current channel selected

#define MAX_CHANNELS sizeof(channelNames) / sizeof(channelNames[0])  // just to determine how many channels are in channelNames array

// map channelNumbers used to convert channelName into channelNumber
// This map is initialized in "setupChannelNumbers()" function by using the "channelNames" array
std::map<String, unsigned int> channelNumbers;

void setupChannelNumbers() {
  for (unsigned int i=0; i < MAX_CHANNELS; i++) {
    channelNumbers[channelNames[i]] = i;
  }
}

bool onChangeChannel(const String &deviceId, String &channel) {
  tvChannel = channelNumbers[channel]; // save new channelNumber in tvChannel variable
  Serial.printf("Change channel to \"%s\" (channel number %d)\r\n", channel.c_str(), tvChannel);
  return true;
}
//! [onChangeChannel]

//! [onChangeChannelNumber]
// channelNames used to convert channelNumber into channelName
// please put in your TV channel names
// channel numbers starts counting from 0!
// so "ZDF" is channel 2
const char* channelNames[] = {
  "A/V",
  "ard", 
  "ZDF", 
  "n. d. r.", 
  "kabel eins", 
  "VOX", 
  "Sat.1", 
  "ProSieben", 
  "rtl", 
  "RTL II", 
  "SUPER RTL", 
  "KiKA"
};

int tvChannel; // current channel selected

#define MAX_CHANNELS sizeof(channelNames) / sizeof(channelNames[0])  // just to determine how many channels are in channelNames array

// map channelNumbers used to convert channelName into channelNumber
// This map is initialized in "setupChannelNumbers()" function by using the "channelNames" array
std::map<String, unsigned int> channelNumbers;

void setupChannelNumbers() {
  for (unsigned int i=0; i < MAX_CHANNELS; i++) {
    channelNumbers[channelNames[i]] = i;
  }
}

bool onChangeChannelNumber(const String& deviceId, int channelNumber, String& channelName) {
  tvChannel = channelNumber; // update tvChannel to new channel number
  if (tvChannel < 0) tvChannel = 0;
  if (tvChannel > MAX_CHANNELS-1) tvChannel = MAX_CHANNELS-1;

  channelName = channelNames[tvChannel]; // return the channelName

  Serial.printf("Change to channel to %d (channel name \"%s\")\r\n", tvChannel, channelName.c_str());
  return true;
}
//! [onChangeChannelNumber]


//! [onSkipChannels]
// channelNames used to convert channelNumber into channelName
// please put in your TV channel names
// channel numbers starts counting from 0!
// so "ZDF" is channel 2
const char* channelNames[] = {
  "A/V",
  "ard", 
  "ZDF", 
  "n. d. r.", 
  "kabel eins", 
  "VOX", 
  "Sat.1", 
  "ProSieben", 
  "rtl", 
  "RTL II", 
  "SUPER RTL", 
  "KiKA"
};

int tvChannel; // current channel selected

#define MAX_CHANNELS sizeof(channelNames) / sizeof(channelNames[0])  // just to determine how many channels are in channelNames array

// map channelNumbers used to convert channelName into channelNumber
// This map is initialized in "setupChannelNumbers()" function by using the "channelNames" array
std::map<String, unsigned int> channelNumbers;

void setupChannelNumbers() {
  for (unsigned int i=0; i < MAX_CHANNELS; i++) {
    channelNumbers[channelNames[i]] = i;
  }
}

bool onSkipChannels(const String &deviceId, const int channelCount, String &channelName) {
  tvChannel += channelCount; // calculate new channel number
  if (tvChannel < 0) tvChannel = 0;
  if (tvChannel > MAX_CHANNELS-1) tvChannel = MAX_CHANNELS-1;
  channelName = String(channelNames[tvChannel]); // return channel name

  Serial.printf("Skip channel: %i (number: %i / name: \"%s\")\r\n", channelCount, tvChannel, channelName.c_str());

  return true;
}
//! [onSkipChannels]


//! [onSelectInput]
bool onSelectInput(const String &deviceId, String &input) {
  Serial.printf("Device %s input changed to %s\r\n", deviceId.c_str(), input.c_str());
  return true;
}
//! [onSelectInput]

//! [onConnected]
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
//! [onConnected]

//! [onDisconnected]
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
//! [onDisconnected]

//! [onSetPosition]
bool onSetPosition(const String &deviceId, int &position) {
  Serial.printf("Device %s set position to %d\r\n", deviceId.c_str(), position);
  return true;
}
//! [onSetPosition]

//! [onAdjustPosition]
int blindsPosition;

bool onAdjustPosition(const String &deviceId, int &positionDelta) {
  blindsPosition += positionDelta;
  Serial.printf("Device %s position changed about %i to %d\r\n", deviceId.c_str(), positionDelta, blindsPosition);
  positionDelta = blindsPosition;
  return true;
}
//! [onAdjustPosition]

//! [onDoorState]
bool onDoorState(const String &deviceId, bool &doorState) {
  Serial.printf("Device %s garage door is now %s\r\n", deviceId.c_str(), doorState?"closed":"open");
  return true;
}
//! [onDoorState]

//! [onKeystroke]
bool onKeystroke(const String& deviceId, String &keystroke) {
  Serial.printf("Device %s, key %s pressed\r\n", deviceId.c_str(), keystroke.c_str());
  return true;
}
//! [onKeystroke]

//! [onSetPercentage]
bool onSetPercentage(const String &deviceId, int &percentage) {
  Serial.printf("Device %s percentage %d\r\n", deviceId.c_str(), percentage);
  return true; // request handled properly
}
//! [onSetPercentage]

//! [onAdjustPercentage]
int absolutePercentage;

bool onAdjustPercentage(const String &deviceId, int &percentageDelta) {
  absolutePercentage += percentageDelta; // calculate absolute percentage
  Serial.printf("Device %s percentage changed about %i to %d\r\n", deviceId.c_str(), percentageDelta, absolutePercentage);
  percentageDelta = absolutePercentage; // return absolute percentage
  return true;                     // request handled properly
}
//! [onAdjustPercentage]
