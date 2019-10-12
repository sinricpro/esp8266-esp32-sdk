
# SinricPro (ESP8266 / ESP32 SDK)

## usage
#### include SinricPro-Library (SinricPro.h) and SinricPro-Device-Libraries (eg. SinricProSwitch.h) 
```C++
#include <SinricPro.h>
#include <SinricProSwitch.h>
```
#### Define your credentials from SinricPro-Portal (portal.sinric.pro)
```C++
#define socketAuthToken	"your-socket-auth-token"
#define signingKey  	"your-signing-key"
#define myDeviceId  	"your-device-id"
```
#### define callback routine(s)
```C++
bool onPowerState(const String deviceId, bool &state) {
  Serial.printf("device %s turned %s\r\n, deviceId.c_str(), state?"on":"off");
  return true; // indicate that callback handled correctly
}
```
#### In setup()
```C++
  SinricPro.begin(socketAuthToken, signingKey);
  // create a switch
  SinricProSwitch mySwitch(myDeviceId);
  // set callback function
  mySwitch.onPowerState(onPowerState);
  // add switch to SinricPro  
  SinricPro.add(mySwitch);
```
  
#### In loop()
```C++
  SinricPro.handle();
```
# Devices

## Switch
Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendPowerStateEvent](#sendpowerstateevent)

---

## Dimmable Switch
Callbacks
- [onPowerState](#onpowerstate)
- [onPowerLevel](#onpowerlevel)
- [onAdjustPowerLevel](#onadjustpowerlevel)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendPowerLevelEvent](#sendpowerlevelevent)
---

## Light

Callbacks
- [onPowerState](#onpowerstate)
- [onBrightness](#onbrightness)
- [onAdjustBrightness](#onadjustbrightness)
- [onColor](#oncolor)
- [onColorTemperature](#oncolortemperature)
- [onIncreaseColorTemperature](#onincreasecolortemperature)
- [onDecreaseColorTemperature](#ondecreasecolortemperature)

Events 
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendBrightnessEvent](#sendbrightnessevent)
- [sendColorEvent](#sendcolorevent)
- [sendColorTemperatureEvent](#sendcolortemperatureevent)
---
		
## TV

Callbacks
- [onPowerState](#onpowerstate)
- [onChangeChannel](#onchangechannel)
- [onSkipChannels](#onskipchannels)
- [onSelectInput](#onselectinput)
- [onSetVolume](#onsetvolume)
- [onAdjustVolume](#onadjustvolume)
- [onMute](#onmute)
- [onMediaControl](#onmediacontrol)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
---

## Speaker

Callbacks
- [onPowerState](#onpowerstate)
- [onSetVolume](#onsetvolume)
- [onAdjustVolume](#onadjustvolume)
- [onMute](#onmute)
- [onMediaControl](#onmediacontrol)
- [onSetBands](#onsetbands)
- [onAdjustBands](#onadjustbands)
- [onResetBands](#onresetbands)
- [onSetMode](#onsetmode)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
---

## Thermostat

Callbacks
- [onPowerState](#onpowerstate)
- [onTargetTemperature](#ontargettemperature)
- [onThermostatMode](#onthermostatmode)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
---
## Lock

Callbacks
- [onLockState](#onlockstate)

Events
- [sendLockStateEvent](#sendlockstateevent)
---


## Callbacks

### onPowerState
```C++
bool onPowerState(const String deviceId, bool &state)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `state`|  `bool&` | `true`: turn on<br>`false`: turn off | `true`: turned on<br>`false`: turned off | `true` |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Switch](#switch) | [Dimmable Switch](#dimmable-switch) | [Light](#light) | [TV](#tv) | [Speaker](#speaker) | [Thermostat](#thermostat) |

---
### onPowerLevel
```C++
bool onPowerLevel(const String deviceId, int &level)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`level`|`int&`|`0`..`100`|`0`..`100`|`75`|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |
---
| [Dimmable Switch](#dimmable-switch) |

### onAdjustPowerLevel
```C++
bool onAdjustPowerLevel(const String deviceId, int &levelDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`levelDelta`|`int&`|`-100`..`100`|`0`..`100`|*Input*:`-25`(reduce level about 25)<br>*Output*:`50` (new level is 50)<br>(*see note*)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note
*Input*: relative value between `-100` and `100`<br>
*Output*: absolute value between `0` and `100`

| [Dimmable Switch](#dimmable-switch) |

---
### onBrightness
```C++
bool onBrightness(const String deviceId, int &brightness)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`brightness`|`int&`|`0`..`100`|`0`..`100`|`75`|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Light](#light) |

---
### onAdjustBrightness
```C++
bool onAdjustBrightness(const String deviceId, int &brightnessDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` | `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`brightnessDelta`|`int&`|`-100`..`100`|`0`..`100`|Input*:`-25`(reduce brightness about 25)<br>*Output*:`50` (new brightness is 50)<br>(*see note*)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note
*Input*: relative value between `-100` and `100`<br>
*Output*: absolute value between `0` and `100`

| [Light](#light) |

---

### onColor
```C++
bool onColor(const String deviceId, byte &r, byte &g, byte &b)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `r`|`byte&`|`0`..`255` | `0`..`255` | `72` (red value) |
| `g`|`byte&`|`0`..`255` | `0`..`255` | `192` (green value) |
| `b`|`byte&`|`0`..`255` | `0`..`255` | `37` (blue value) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Light](#light) |

---


### onColorTemperature

```C++
bool onColorTemperature(const String deviceId, int &colorTemperature)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`colorTemperature` | `int&` | `2200`,`2700`, `4000`, `5500`, `7000` | `2200`,`2700`, `4000`, `5500`, `7000` | `2700` (soft white) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Light](#light) |

---

### onIncreaseColorTemperature
```C++
bool onIncreaseColorTemperature(const String deviceId, int &colorTemperature)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`colorTemperature` | `int&` | ---<br>(see note) | `2200`,`2700`, `4000`, `5500`, `7000` | `4000` (white) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note
*Input*: ---
*Output*: new color temperature [`2200`,`2700`, `4000`, `5500`, `7000`]

| [Light](#light) |

---

### onDecreaseColorTemperature
```C++
bool onDecreaseColorTemperature(const String deviceId, int &colorTemperature)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`colorTemperature` | `int&` | ---<br>(see note) | `2200`,`2700`, `4000`, `5500`, `7000` | `2200` (warm white) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note
*Input*: ---
*Output*: new color temperature [`2200`,`2700`, `4000`, `5500`, `7000`]

| [Light](#light) |

---
### onChangeChannel
```C++
bool onChangeChannel(const String deviceId, String &channel)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`channel` | `String&` | `channel name` | `channel name` | `"HBO"` (name of a tv channel) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [TV](#tv) | 

---
### onSkipChannels
```C++
bool onSkipChannels(const String deviceId, const int channelCount, String &channelName)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `channelCount` | `const int`| `-n`..`+n` | --- | `-5`|
|`channelName` | `String&` | --- | `channel name` | `"HBO"` (name of the new tv channel) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [TV](#tv) | 

---
### onSelectInput
```C++
bool onSelectInput(const String deviceId, String &input)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`input` | `String&` | `input name` | `input name` | `"HDMI 1"` (name of an input) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [TV](#tv) | 

---
### onSetVolume
```C++
bool onSetVolume(const String deviceId, int &volume)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`volume`|`int&`|`0`..`100`|`0`..`100`|`75`|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [TV](#tv) | [Speaker](#speaker) |

---
### onAdjustVolume
```C++
bool onAdjustVolume(const String deviceId, int&volumeDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`volumeDelta`|`int&`|`-100`..`100`|`0`..`100`|`-10` (*see note*)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note
*Input*: relative value between -100 and 100
*Output*: absolute value between 0 and 100

| [TV](#tv) | [Speaker](#speaker) |

---

### onMute
```C++
bool onMute(const String deviceId, bool &mute)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `mute`|  `bool&` | `true`: mute<br>`false`: unmute | `true`: device is muted<br>`false`: device is not muted | `true` |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [TV](#tv) | [Speaker](#speaker) |

---
### onMediaControl
```C++
bool onMediaControl(const String deviceId, String &control)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `control`|  `String&` | `"Play"`, `"Pause"`, `"Stop"`, `"StartOver"`, `"Previous"`, `"Next"`, `"Rewind"`, `"FastForward"`| `"Play"`, `"Pause"`, `"Stop"`, `"StartOver"`, `"Previous"`, `"Next"`, `"Rewind"`, `"FastForward"` | `Pause`: device is paused |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [TV](#tv) | [Speaker](#speaker) |

---
### onSetBands
```C++
bool onSetBands(const String deviceId, const String bands, int &level)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`bands`|`const String`| `BASS`, `MIDRANGE`, `TREBBLE` |---| `BASS` (set bass band)|
|`level`| `int&` |`0`..`n`|`0`..`n`|`5` (set level to 5) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Speaker](#speaker) |

---
### onAdjustBands
```C++
bool onAdjustBands(const String deviceId, const String bands, int &levelDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`bands`|`const String`| `BASS`, `MIDRANGE`, `TREBBLE` |---| `BASS` (adjust BASS band)|
|`levelDelta`| `int&` |`-n`..`+n`|`0`..`n`|*Input*:`-3` (lower level about 3)<br>*Output*:`2` (new level is 2)<br>(see note) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note
*Input*: relative value between `-n` and `+n`
*Output*: absolute value between `0` and `n`

| [Speaker](#speaker) |

---
### onResetBands
```C++
bool onResetBands(const String deviceId, const String bands, int &level)
```

| [Speaker](#speaker) |

---
### onSetMode
```C++
bool onSetMode(const String deviceId, String &mode)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`mode`|`String&`| `MODE` | `MODE`| `"MOVIE"` (set mode to MOVIE)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Speaker](#speaker) |

---
### onTargetTemperature
```C++
bool onTargetTemperatue(const String deviceId, float &temperature)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`temperature`|`float&`| `-n`..`+n` | `-n`..`+n`| `18.2f` (set temperature to 18.2°)<br>(see note)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note
Values are in Celsius or Kelvin (depending on your account settings

| [Thermostat](#thermostat) |

---
### onThermostatMode
```C++
bool onThermostatMode(const String deviceId, String &mode)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`mode`|`String&`| `COOL`, `HEAT`,`AUTO` | `COOL`, `HEAT`,`AUTO`| `AUTO` (set mode to "automatic")|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Thermostat](#thermostat) |

---

### onLockState
```C++
bool onLockState(const String deviceId, bool &state)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`state`|`bool&`| `true`: lock device<br>`false`: unlock device |  `true`: device is locked<br>`false`: device is unlocked | `true` |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

| [Lock](#lock) |

---



## Events
### sendPowerStateEvent
```C++
sendPowerStateEvent(bool state, String cause="PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|--|
| `state` | `bool` | device state |`true`: device is on<br>`false`: device is off | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"`

| [Switch](#switch) | [Dimmable Switch](#dimmable-switch) | [Light](#light) | [TV](#tv) | [Speaker](#speaker) | [Thermostat](#thermostat) |

---
### sendPowerLevelEvent
```C++
sendPowerLevelEvent(int level, String cause="PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `level` | `int` | device power level | `0`..`100`| 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"`

| [Dimmable Switch](#dimmable-switch) |

---



## Dependencies / Needed libraries
[WebSocketsClient](https://github.com/Links2004/arduinoWebSockets)
[ArduinoJson](https://github.com/bblanchon/ArduinoJson)
[NTPClient](https://github.com/arduino-libraries/NTPClient)
