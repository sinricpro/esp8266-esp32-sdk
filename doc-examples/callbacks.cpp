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


//! [onSetVolume]
bool onSetVolume(const String &deviceId, int &volume) {
  Serial.printf("Device %s volume set to %d\r\n", deviceId.c_str(), volume);
  return true; // request handled properly
}
//! [onSetVolume]

//! [onAdjustVolume]
int absoluteVolume;

bool onAdjustVolume(const String &deviceId, int &volumeDelta) {
  absoluteVolume += volumeDelta;  // calculate absolute volume
  Serial.printf("Device %s volume changed about %i to %d\r\n", deviceId.c_str(), volumeDelta, absoluteVolume);
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
  Serial.printf("Device %s: %s\r\n", deviceId.c_str(), control);
  return true; // request handled properly
}
//! [onMediaControl]

//! [onSetBands]
bool onSetBands(const String &deviceId, String &bands, int &level) {
  Serial.printf("Device %s bands %s set to %d\r\n", deviceId.c_str(), bands.c_str(), level);
  return true; // request handled properly
}
//! [onSetBands]

//! [onAdjustBands]
int globalBass;
int globalMidrange;
int globalTrebble;

bool onAdjustBands(const String &deviceId, String &bands, int &levelDelta) {
  if (bands == "BASS") {
    globalBass += levelDelta; // calculate absolute bass level
    levelDelta = globalBass; // return absolute bass level
  }
  if (bands == "MIDRANGE") {
    globalMidrange += levelDelta; // calculate absolute midrange level
    levelDelta = globalMidrange; // return absolute midrange level
  }
  if (bands == "TREBBLE") {
    globalMidrange += levelDelta; // calculate absolute trebble level
    levelDelta = globalMidrange; // return absolute trebble level
  }
  Serial.printf("Device %s bands set to\r\n - BASS: %d\r\n - MIDRANGE: %d\r\n - TREBBLE: %d\r\n", deviceId.c_str(), globalBass, globalMidrange, globalTrebble);
  return true; // request handled properly
}
//! [onAdjustBands]


//! [onResetBands]
int globalBass;
int globalMidrange;
int globalTrebble;

bool onAdjustBands(const String &deviceId, String &bands, int &level) {
  if (bands == "BASS") {
    globalBass = 0; // reset bass level to 0
    level = globalBass; // return bass level
  }
  if (bands == "MIDRANGE") {
    globalMidrange = 0; // reset midrange level to 0
    level = globalMidrange; // return midrange level
  }
  if (bands == "TREBBLE") {
    globalMidrange = 0; // reset trebble level to 0
    level = globalMidrange; // return trebble level
  }
  Serial.printf("Device %s bands reset to\r\n - BASS: %d\r\n - MIDRANGE: %d\r\n - TREBBLE: %d\r\n", deviceId.c_str(), globalBass, globalMidrange, globalTrebble);
  return true; // request handled properly
}
//! [onResetBands]


//! [onSetMode]
bool onSetMode(const String &deviceId, String &mode) {
  Serial.printf("Device %s is set to mode %s\r\n", deviceId.c_str(), mode);
  return true; // request handled properly
}
//! [onSetMode]

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

