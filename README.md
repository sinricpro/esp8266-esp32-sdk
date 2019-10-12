
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
- [sendTemperatureEvent](#sendtemperatureevent)
- [sendTargetTemperatureEvent](#sendtargettemperatureevent)
- [sendThermostatModeEvent](#sendthermostatmodeevent)

---
## Lock

Callbacks
- [onLockState](#onlockstate)

Events
- [sendLockStateEvent](#sendlockstateevent)
---
## Doorbell

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendDoorbellEvent](#senddoorbellevent)
---
## Temperaturesensor

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendTemperatureEvent](#sendtemperatureevent)
---
## MotionSensor

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendMotionEvent](#sendmotionevent)
---

## ContactSensor

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendContactEvent](#sendcontactevent)
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

Devices: | [Switch](#switch) | [Dimmable Switch](#dimmable-switch) | [Light](#light) | [TV](#tv) | [Speaker](#speaker) | [Thermostat](#thermostat) | [Doorbell](#doorbell) | [Temperaturesensor](#temperaturesensor) | [Thermostat](#thermostat) | [MotionSensor](#motionsensor) | [ContactSensor](#contactsensor) |

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
Devices: | [Dimmable Switch](#dimmable-switch) |

### onAdjustPowerLevel
```C++
bool onAdjustPowerLevel(const String deviceId, int &levelDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`levelDelta`|`int&`|`-100`..`100`|`0`..`100`|*Input*:`-25`(reduce level about 25)<br>*Output*:`50` (new level is 50)<br>(*see note*)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note<br>
*Input*: relative value between `-100` and `100`<br>
*Output*: absolute value between `0` and `100`

Devices: | [Dimmable Switch](#dimmable-switch) |

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

Devices: | [Light](#light) |

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

Note<br>
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

Devices: | [Light](#light) |

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

Devices: | [Light](#light) |

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

Note<br>
*Input*: ---<br>
*Output*: new color temperature [`2200`,`2700`, `4000`, `5500`, `7000`]

Devices: | [Light](#light) |

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

Note<br>
*Input*: ---<br>
*Output*: new color temperature [`2200`,`2700`, `4000`, `5500`, `7000`]

Devices: | [Light](#light) |

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

Devices: | [TV](#tv) | 

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

Devices: | [TV](#tv) | 

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

Devices: | [TV](#tv) | 

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

Devices: | [TV](#tv) | [Speaker](#speaker) |

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

Note<br>
*Input*: relative value between `-100` and `100`<br>
*Output*: absolute value between `0` and `100`

Devices: | [TV](#tv) | [Speaker](#speaker) |

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

Devices: | [TV](#tv) | [Speaker](#speaker) |

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

Devices: | [TV](#tv) | [Speaker](#speaker) |

---
### onSetBands
```C++
bool onSetBands(const String deviceId, const String bands, int &level)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`bands`|`const String`| `"BASS"`, `"MIDRANGE"`, `"TREBBLE"` |---| `"BASS"` (set bass band)|
|`level`| `int&` |`0`..`n`|`0`..`n`|`5` (set level to 5) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [Speaker](#speaker) |

---
### onAdjustBands
```C++
bool onAdjustBands(const String deviceId, const String bands, int &levelDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`bands`|`const String`| `"BASS"`, `"MIDRANGE"`, `"TREBBLE"` |---| `"BASS"` (adjust BASS band)|
|`levelDelta`| `int&` |`-n`..`+n`|`0`..`n`|*Input*:`-3` (lower level about 3)<br>*Output*:`2` (new level is 2)<br>(see note) |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note<br>
*Input*: relative value between `-n` and `+n`<br>
*Output*: absolute value between `0` and `n`

Devices: | [Speaker](#speaker) |

---
### onResetBands
```C++
bool onResetBands(const String deviceId, const String bands, int &level)
```

Devices: | [Speaker](#speaker) |

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

Devices: | [Speaker](#speaker) |

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

Note<br>
Values are in Celsius or Kelvin (depending on your account settings)

Devices: | [Thermostat](#thermostat) |

---
### onThermostatMode
```C++
bool onThermostatMode(const String deviceId, String &mode)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`mode`|`String&`| `COOL`, `"HEA"T`,`"AUTO"` | `"COOL"`, `"HEAT"`,`"AUTO"`| `"AUTO"` (set mode to "automatic")|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [Thermostat](#thermostat) |

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

Devices: | [Lock](#lock) |

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

Devices: | [Switch](#switch) | [Dimmable Switch](#dimmable-switch) | [Light](#light) | [TV](#tv) | [Speaker](#speaker) | [Temperaturesensor](#temperaturesensor) | [Thermostat](#thermostat) | [MotionSensor](#motionsensor) | [ContactSensor](#contactsensor) |

---
### sendPowerLevelEvent
```C++
sendPowerLevelEvent(int level, String cause="PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `level` | `int` | device power level | `0`..`100`| 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Dimmable Switch](#dimmable-switch) |

---
### sendBrightnessEvent
```C++
void sendBrightnessEvent(int brightness, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `brightness` | `int` | brightness level | `0`..`100`| 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Light](#light) |

---
### sendColorEvent
```C++
void sendColorEvent(byte r, byte g, byte b, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `r` | `byte` | red value | `0`..`255` | 
| `g` | `byte` | green value | `0`..`255` | 
| `b` | `byte` | blue value | `0`..`255` | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Light](#light) |

---
### sendColorTemperatureEvent
```C++
void sendColorTemperatureEvent(int colorTemperature, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `colorTemperature` | `int` | color temperature | `2200`, `2700`, `4000`, `5500`, `7000` | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Light](#light) |

---
### sendDoorbellEvent
```C++
void sendDoorbellEvent(String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Doorbell](#doorbell) |

---
### sendTemperatureEvent
```C++
void sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `temperature` | `float` | temperature | `-n`..`n` | 
| `humidity` | `float` | (optional) humidity | `-n`..`+n`<br>(`-1` if not supported) | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PERIODIC_POLL"` |

Devices: | [Temperaturesensor](#temperaturesensor) | [Thermostat](#thermostat) |

---
### sendTargetTemperatureEvent
```C++
void sendTargetTemperatureEvent(float temperature, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `temperature` | `float` | temperature | `-n`..`n` | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PERIODIC_POLL"` |

Devices: | [Thermostat](#thermostat) |

---
### sendThermostatModeEvent
```C++
void sendThermostatModeEvent(String thermostatMode, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `thermostatMode` | `String` | thermostat mode | `"COOL"`, `"HEAT"`, `"AUTO"` | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Thermostat](#thermostat) |

---
### sendMotionEvent
```C++
void sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `detected` | `bool` | `true`: motion detected<br>`false`: motion not detected (should be sent if motion not happens any longer) | `true`, `false` | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [MotionSensor](#motionsensor) |

---
### sendContactEvent
```C++
void sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `detected` | `bool` | `true`: contact is closed<br>`false`: contact is open | `true`, `false` | 
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [ContactSensor](#contactsensor) |

---



## Dependencies / Needed libraries
[ArduinoJson](https://github.com/bblanchon/ArduinoJson)<br>
[WebSocketsClient](https://github.com/Links2004/arduinoWebSockets)<br>
[NTPClient](https://github.com/arduino-libraries/NTPClient)
