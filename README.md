
# SinricPro (ESP8266 / ESP32 SDK)
## Version 2.3.0
## Installation

### VS Code & PlatformIO:
1. Install [VS Code](https://code.visualstudio.com/)  
2. Install [PlatformIO](https://platformio.org/platformio-ide)  
3. Install **SinricPro** library by using [Library Manager](https://docs.platformio.org/en/latest/librarymanager/)  
4. Use included [platformio.ini](https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/pio-examples/switch/platformio.ini) files from [examples](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/pio-examples) to ensure that all dependent libraries will installed automaticly.

![sinricpro library manager](https://raw.githubusercontent.com/sinricpro/images/master/platformio-install-sinricpro.png)

### ArduinoIDE
1. Open Library Manager (*Tools / Manage Libraries*)  
2. Search for *SinricPro* and click *Install*  
3. Repeat step 2 for all [dependent libraries](#dependencies)!
4. Open example in ArduinoIDE (*File / Examples / SinricPro / ...*)  

![ArduinoIDE Library Manager](https://raw.githubusercontent.com/sinricpro/images/master/ArduinoIDE-Library-Manager.png)

---

## Examples
|PlatformIO|Arduino|
|--|--|
|[Switch](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/pio-examples/switch)  |[Switch](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Switch)|
|[Doorbell](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/pio-examples/doorbell)|[Doorbell](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/doorbell)|

---

## Dependencies
[ArduinoJson](https://github.com/bblanchon/ArduinoJson) (Version 6.12.0)   
[WebSocketsClient](https://github.com/Links2004/arduinoWebSockets) (Version 2.2.0)  

---
## Usage
#### Include SinricPro-Library (SinricPro.h) and SinricPro-Device-Libraries (eg. SinricProSwitch.h)
```C++
#include <SinricPro.h>
#include <SinricProSwitch.h>
```
#### Define your credentials from SinricPro-Portal (portal.sinric.pro)
```C++
#define APP_KEY	    "YOUR-APP-KEY"
#define APP_SECRET  "YOUR-APP-SECRET"
#define SWITCH_ID   "YOUR-SWITCH-DEVICEID" // Note: This is not device name! This is device id like "5da8xxxxxxxxxxxxxxxxxxxx"
```

#### Define callback routine(s)
```C++
bool onPowerState(const String deviceId, bool &state) {
  Serial.printf("device %s turned %s\r\n, deviceId.c_str(), state?"on":"off");
  return true; // indicate that callback handled correctly
}
```
#### In setup()
```C++
  // create and add a switch to SinricPro
  SinricProSwitch& mySwitch = SinricPro.add<SinricProSwitch>(SWITCH_ID);
  // set callback function
  mySwitch.onPowerState(onPowerState);
  // startup SinricPro
  SinricPro.begin(APP_KEY, APP_SECRET);

```

#### In loop()
```C++
  SinricPro.handle();
```

---
## How to add a device?
Syntax is  
```C++
  DeviceType& myDevice = SinricPro.add<DeviceType>(DEVICE_ID);
```
Example  
```C++
  SinricProSwitch& mySwitch = SinricPro.add<SinricProSwitch>("YOUR-SWITCH-ID-HERE");
```

---
## How to retrieve a device for sending an event?
Syntax is  
```C++
  DeviceType& myDevice = SinricPro[DEVICE_ID];
```
Example 1
```C++
  SinricProDoorbell& myDoorbell = SinricPro["YOUR-DOORBELL-ID-HERE"];
  myDoorbell.sendDoorbellEvent();
```

Example 2 (alternatively)
```C++
  SinricPro["YOUR-DOORBELL-ID-HERE"].as<SinricProDoorbell>().sendDoorbellEvent();
```


---

# Devices
[Switch](#switch) | [Dimmable Switch](#dimmable-switch) | [Light](#light) | [TV](#tv) | [Speaker](#speaker) | [Thermostat](#thermostat) | [Fan (US)](#fan-us) | [Fan (non US)](#fan-non-us) | [Lock](#lock) | [Doorbell](#doorbell) | [TemperatureSensor](#temperaturesensor) | [MotionSensor](#motionsensor) | [ContactSensor](#contactsensor) | [Window AC Unit](#window-ac-unit)

---
## Switch
Defined in [SinricProSwitch.h](/src/SinricProSwitch.h)  

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendPowerStateEvent](#sendpowerstateevent)

---

## Dimmable Switch
Defined in [SinricProDimSwitch.h](/src/SinricProDimSwitch.h)

Callbacks
- [onPowerState](#onpowerstate)
- [onPowerLevel](#onpowerlevel)
- [onAdjustPowerLevel](#onadjustpowerlevel)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendPowerLevelEvent](#sendpowerlevelevent)
---

## Light
Defined in [SinricProLight.h](/src/SinricProLight.h)  

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
Defined in [SinricProTV.h](/src/SinricProTV.h)  

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
- [sendVolumeEvent](#sendvolumeevent)
- [sendMuteEvent](#sendmuteevent)
- [sendMediaControlEvent](#sendmediacontrolevent)
- [sendSelectInputEvent](#sendselectinputevent)
- [sendChangeChannelEvent](#sendchangechannelevent)
---

## Speaker
Defined in [SinricProSpeaker.h](/src/SinricProSpeaker.h)

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
- [sendVolumeEvent](#sendvolumeevent)
- [sendMuteEvent](#sendmuteevent)
- [sendMediaControlEvent](#sendmediacontrolevent)
- [sendSetBandsEvent](#sendsetbandsevent)
- [sendModeEvent](#sendmodeevent)
---

## Thermostat
Defined in [SinricProThermostat.h](/src/SinricProThermostat.h)  

Callbacks
- [onPowerState](#onpowerstate)
- [onTargetTemperature](#ontargettemperature)
- [onAdjustTargetTemperature](#onadjusttargettemperature)
- [onThermostatMode](#onthermostatmode)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendTemperatureEvent](#sendtemperatureevent)
- [sendTargetTemperatureEvent](#sendtargettemperatureevent)
- [sendThermostatModeEvent](#sendthermostatmodeevent)

---
## Fan (US)
Defined in [SinricProFanUS.h](/src/SinricProFanUS.h)  

Callbacks
- [onPowerState](#onpowerstate)
- [onRangeValue](#onrangevalue)
- [onAdjustRangeValue](#onadjustrangevalue)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendRangeValueEvent](#sendrangevalueevent)

---
## Fan (non US)
Defined in [SinricProFan.h](/src/SinricProFan.h)  

Callbacks
- [onPowerState](#onpowerstate)
- [onPowerLevel](#onpowerlevel)
- [onAdjustPowerLevel](#onadjustpowerlevel)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendPowerLevelEvent](#sendpowerlevelevent)
---

## Lock
Defined in [SinricProLock.h](/src/SinricProLock.h)  

Callbacks
- [onLockState](#onlockstate)

Events
- [sendLockStateEvent](#sendlockstateevent)
---
## Doorbell
Defined in [SinricProDoorbell.h](/src/SinricProDoorbell.h)

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendDoorbellEvent](#senddoorbellevent)
---
## Temperaturesensor
Defined in [SinricProTemperatureSensor.h](/src/SinricProTemperaturesensor.h)  

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendTemperatureEvent](#sendtemperatureevent)
---
## MotionSensor
Defined in [SinricProMotionsensor.h](/src/SinricProMotionsensor.h)  

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendMotionEvent](#sendmotionevent)
---

## ContactSensor
Defined in [SinricProContactsensor.h](/src/SinricProContactsensor.h)  

Callbacks
- [onPowerState](#onpowerstate)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendContactEvent](#sendcontactevent)
---

## Window AC Unit
Defined in [SinricProWindowAC.h](/src/SinricProWindowAC.h)  

Callbacks
- [onPowerState](#onpowerstate)
- [onTargetTemperature](#ontargettemperature)
- [onAdjustTargetTemperature](#onadjusttargettemperature)
- [onThermostatMode](#onthermostatmode)
- [onRangeValue](#onrangevalue)
- [onAdjustRangeValue](#onadjustrangevalue)

Events
- [sendPowerStateEvent](#sendpowerstateevent)
- [sendTemperatureEvent](#sendtemperatureevent)
- [sendTargetTemperatureEvent](#sendtargettemperatureevent)
- [sendThemostatModeEvent](#sendthermostatmodeevent)
- [sendRangeValueEvent](#sendrangevalueevent)
---

# Callbacks

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

Devices: | [Light](#light) |

---

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

Devices: | [Dimmable Switch](#dimmable-switch) | [Fan (non US)](#fan-non-us) |

---

### onAdjustRangeValue
```C++
bool onAdjustPowerLevel(const String deviceId, int &rangeValueDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `rangeValueDelta` | `int&` | `-n`..`n` | `0`..`n` |*Input*:`-2`(reduce rangeValue about 2)<br>*Output*:`1` (new range Value is 1)<br>(*see note*)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note<br>
*Input*: relative value between `-n` and `n`<br>
*Output*: absolute value between `0` and `n`

Devices: | [Fan (US)](#fan-us) | [Window AC Unit](#window-ac-unit) |

---

### onAdjustTargetTemperature
```C++
bool onAdjustTargetTemperature(const String deviceId, float &temperatureDelta)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `temperatureDelta` | `float&` | `-n`..`n` | `0`..`n` |*Input*:`-2`(reduce temperature about 2°)<br>*Output*:`1` (new temperature Value is 1)<br>(*see note*)|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Note<br>
*Input*: relative value between `-n` and `n`<br>
*Output*: absolute value between `0` and `n`

Devices: | [Thermostat](#thermostat) | [Window AC Unit](#window-ac-unit) |

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

### onPowerLevel
```C++
bool onPowerLevel(const String deviceId, int &level)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`level`|`int&`|`0`..`100`|`0`..`100`|`75`|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [Dimmable Switch](#dimmable-switch) | [Fan (non US)](#fan-non-us) |

---

### onPowerState
```C++
bool onPowerState(const String deviceId, bool &state)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
| `state`|  `bool&` | `true`: turn on<br>`false`: turn off | `true`: turned on<br>`false`: turned off | `true` |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [Switch](#switch) | [Dimmable Switch](#dimmable-switch) | [Light](#light) | [TV](#tv) | [Speaker](#speaker) | [Thermostat](#thermostat) | [Doorbell](#doorbell) | [Temperaturesensor](#temperaturesensor) | [Thermostat](#thermostat) | [MotionSensor](#motionsensor) | [ContactSensor](#contactsensor) | [Fan (non US)](#fan-non-us) | [Fan (US)](#fan-us) |

---

### onRangeValue
```C++
bool onRangeValue(const String deviceId, int &rangeValue)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`rangeValue`|`int&`| `0`..`n` | `0`..`n` | `3` (device is set to 3)
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [Fan (US)](#fan-us) | [Window AC Unit](#window-ac-unit) |

---

### onResetBands
```C++
bool onResetBands(const String deviceId, const String bands, int &level)
```

Devices: | [Speaker](#speaker) |

---

### onSelectInput
```C++
bool onSelectInput(const String deviceId, String &input)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`input` | `String&` | `input name` | `input name` | `"AUX 1"`, `"AUX 2"`, `"AUX 3"`, `"AUX 4"`, `"AUX 5"`, `"AUX 6"`, `"AUX 7"`, `"BLURAY"`, `"CABLE"`, `"CD"`, `"COAX 1"`,`"COAX 2"`, `"COMPOSITE 1"`, `"DVD"`, `"GAME"`, `"HD RADIO"`, `"HDMI 1"`, `"HDMI 2"`, `"HDMI 3"`, `"HDMI 4"`, `"HDMI 5"`, `"HDMI 6"`, `"HDMI 7"`, `"HDMI 8"`, `"HDMI 9"`, `"HDMI 10"`, `"HDMI ARC"`, `"INPUT 1"`, `"INPUT 2"`, `"INPUT 3"`, `"INPUT 4"`, `"INPUT 5"`, `"INPUT 6"`, `"INPUT 7"`, `"INPUT 8"`, `"INPUT 9"`, `"INPUT 10"`, `"IPOD"`, `"LINE 1"`, `"LINE 2"`, `"LINE 3"`, `"LINE 4"`, `"LINE 5"`, `"LINE 6"`, `"LINE 7"`, `"MEDIA PLAYER"`, `"OPTICAL 1"`, `"OPTICAL 2"`, `"PHONO"`, `"PLAYSTATION"`, `"PLAYSTATION 3"`, `"PLAYSTATION 4"`, `"SATELLITE"`, `"SMARTCAST"`, `"TUNER"`, `"TV"`, `"USB DAC"`, `"VIDEO 1"`, `"VIDEO 2"`, `"VIDEO 3"`, `"XBOX"` |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [TV](#tv) |

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

### onSetMode
```C++
bool onSetMode(const String deviceId, String &mode)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`mode`|`String&`| `MODE` | `MODE`| `"MOVIE"`, `"MUSIC"`, `"NIGHT"`, `"SPORT"`, `"TV"` |
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [Speaker](#speaker) |

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

Devices: | [Thermostat](#thermostat) | [Window AC Unit](#window-ac-unit) |

---

### onThermostatMode
```C++
bool onThermostatMode(const String deviceId, String &mode)
```
| parameter | type|  input value | output value | example |
|--|--|--|--|-- |
| `deviceId` |  `const String` | `deviceId` | --- | `"5d8f5ade41307b450335925d"` |
|`mode`|`String&`| `"COOL"`, `"HEAT"`, `"AUTO"` | `"COOL"`, `"HEAT"`,`"AUTO"`| `"AUTO"` (set mode to "automatic")|
| `[return]`|  `bool` | --- | `true`: request handled properly<br>`false`: error | `true` |

Devices: | [Thermostat](#thermostat) | [Window AC Unit](#window-ac-unit) |

---

# Events

### sendBandsEvent
```C++
bool sendBandsEvent(String bands, int level, String cause = "PHYSICAL_INTERACTION")"PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `bands` | `String` | selected band | `"BASS"`, `"MIDRANGE"`,`"TREBBLE"` |
| `level` | `int` | report new band level | `0`..`n` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Speaker](#speaker) |

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

### sendChangeChannelEvent
```C++
bool sendChangeChannelEvent(String channelName, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `channelName` | `String` | report new channel | example: `"HBO"` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [TV](#tv) |

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

### sendContactEvent
```C++
void sendContactEvent(bool detected, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `detected` | `bool` | report contact state | `true`: contact is closed<br>`false`: contact is open |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [ContactSensor](#contactsensor) |

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

### sendMediaControlEvent
```C++
bool sendMediaControlEvent(String mediaControl, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `mediaControl` | `String` | report a media event | `"Play"`, `"Pause"`, `"Stop"`, `"StartOver"`, `"Previous"`, `"Next"`, `"Rewind"`, `"FastForward"` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [TV](#tv) | [Speaker](#speaker) |

---

### sendModeEvent
```C++
bool sendModeEvent(String mode, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `mode` | `String` | report new mode | `"MOVIE"`, `"MUSIC"`, `"NIGHT"`, `"SPORT"`, `"TV"` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Speaker](#speaker) |

---

### sendMotionEvent
```C++
void sendMotionEvent(bool detected, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `detected` | `bool` | report motion event | `true`: motion detected<br>`false`: motion not detected (should be sent if motion not happens any longer) |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [MotionSensor](#motionsensor) |

---

### sendMuteEvent
```C++
bool sendMuteEvent(bool mute, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `mute` | `bool` | report mute event | `true`: device is muted <br>`false`: device is unmuted |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [TV](#tv) | [Speaker](#speaker) |

---

### sendPowerLevelEvent
```C++
sendPowerLevelEvent(int level, String cause="PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `level` | `int` | device power level | `0`..`100`|
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Dimmable Switch](#dimmable-switch) | [Fan (non US)](#fan-non-us) |

---

### sendPowerStateEvent
```C++
sendPowerStateEvent(bool state, String cause="PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|--|
| `state` | `bool` | device state |`true`: device is on<br>`false`: device is off |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"`

Devices: | [Switch](#switch) | [Dimmable Switch](#dimmable-switch) | [Light](#light) | [TV](#tv) | [Speaker](#speaker) | [Temperaturesensor](#temperaturesensor) | [Thermostat](#thermostat) | [MotionSensor](#motionsensor) | [ContactSensor](#contactsensor) | [Fan (non US)](#fan-non-us) | [Fan (US)](#fan-us) |

---

### sendRangeValueEvent
```C++
bool sendRangeValueEvent(int rangeValue, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `rangeValue` | `int` | report actual range value | `0`..`n` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Fan (US)](#fan-us) | [Window AC Unit](#window-ac-unit) |

---

### sendSelectInputEvent
```C++
bool sendSelectInputEvent(String intput, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `input` | `String` | report actual input | `"AUX 1"`, `"AUX 2"`, `"AUX 3"`, `"AUX 4"`, `"AUX 5"`, `"AUX 6"`, `"AUX 7"`, `"BLURAY"`, `"CABLE"`, `"CD"`, `"COAX 1"`,`"COAX 2"`, `"COMPOSITE 1"`, `"DVD"`, `"GAME"`, `"HD RADIO"`, `"HDMI 1"`, `"HDMI 2"`, `"HDMI 3"`, `"HDMI 4"`, `"HDMI 5"`, `"HDMI 6"`, `"HDMI 7"`, `"HDMI 8"`, `"HDMI 9"`, `"HDMI 10"`, `"HDMI ARC"`, `"INPUT 1"`, `"INPUT 2"`, `"INPUT 3"`, `"INPUT 4"`, `"INPUT 5"`, `"INPUT 6"`, `"INPUT 7"`, `"INPUT 8"`, `"INPUT 9"`, `"INPUT 10"`, `"IPOD"`, `"LINE 1"`, `"LINE 2"`, `"LINE 3"`, `"LINE 4"`, `"LINE 5"`, `"LINE 6"`, `"LINE 7"`, `"MEDIA PLAYER"`, `"OPTICAL 1"`, `"OPTICAL 2"`, `"PHONO"`, `"PLAYSTATION"`, `"PLAYSTATION 3"`, `"PLAYSTATION 4"`, `"SATELLITE"`, `"SMARTCAST"`, `"TUNER"`, `"TV"`, `"USB DAC"`, `"VIDEO 1"`, `"VIDEO 2"`, `"VIDEO 3"`, `"XBOX"` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [TV](#tv) |

---

### sendTargetTemperatureEvent
```C++
void sendTargetTemperatureEvent(float temperature, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `temperature` | `float` | temperature | `-n`..`n` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [Thermostat](#thermostat) | [Window AC Unit](#window-ac-unit) |

---

### sendTemperatureEvent
```C++
void sendTemperatureEvent(float temperature, float humidity = -1, String cause = "PERIODIC_POLL")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `temperature` | `float` | temperature | `-n`..`n` |
| `humidity` | `float` | (optional) humidity | `0`..`+n`<br>(`-1` if not supported) |
| `cause` | `String` | (optional) describing why this is event ocours | `"PERIODIC_POLL"` |

Devices: | [Temperaturesensor](#temperaturesensor) | [Thermostat](#thermostat) | [Window AC Unit](#window-ac-unit) |

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

### sendVolumeEvent
```C++
bool sendVolumeEvent(int volume, String cause = "PHYSICAL_INTERACTION")
```
| parameter| type | description | value |
|--|--|--|:--:|
| `volume` | `int` | report actual volume level| `0`..`100` |
| `cause` | `String` | (optional) describing why this is event ocours | `"PHYSICAL_INTERACTION"` |

Devices: | [TV](#tv) | [Speaker](#speaker) |

---
