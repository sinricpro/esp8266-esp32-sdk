# Changelog

## Version 2.10.0
- Feature: Push Notification.

## Version 2.9.17
- Fixed: Case sensivitiy include [#251](https://github.com/sinricpro/esp8266-esp32-sdk/issues/251)
- Fixed: platformio.ini example [#252](https://github.com/sinricpro/esp8266-esp32-sdk/issues/252)

## Version 2.9.16
- Fixed: Issue in RangeController
  For relative changes, onRangeValue was called instead of onAdjustRangeValue.

## Version 2.9.15
- Fixed: Namespace issues with custom devices

## Version 2.9.14
- Fixed: Cannot connect to the server.

## Version 2.9.13
- "firmwareVersion" WebSocket header added.
- "version" WebSocket header renamed to "SDKVersion"

## Version 2.9.12
- Fixed: Namespace implementation
- Fixed: WiFi connection status check

## Version 2.9.11
- Add missing CAMERA_MODEL_AI_THINKER in Camera example
## Version 2.9.10
- Fixed event limitation in PowerLevelController
- Example "RGB_LED_Stripe_5050.ino" extended to support color temperature
- Example "ACUnit.ino" extended to support onRangeValue / adjustRangeValue

## Version 2.9.9
- Fixed include instruction in EventLimiter.h (uppercase / lowercase issue)

## Version 2.9.8
- Fixed SinricProBlinds

## Version 2.9.7
- Fixed SinricProDimSwitch (include order)

## Version 2.9.6
### New
- Namespace implemented
  Each version gets its own namespace in the form of SINRICPRO_X_Y_Z (SINRICPRO_2_9_6)
- SinricProDevice: implemented registerRequestHandler (used by capabilities to register their requestHandler functions)
- Implemented SinricProStrings to avoid unnecessary string duplication and move the strings into PROGMEM.
- New temperature sensors example added HTU21D & AHT10 (thanks to [jpa-welten](https://github.com/jpa-welten))
### Changes
- Removed types: `DeviceId`, `AppKey` and `AppSecret`. These are now again of type `String`.
- Include guard is now based on `#pragma once` instead of `#ifndef`, `#define` and `#endif`
- Event-Limiting system changed\
Attempting to send events shorter than these minimum waiting times will add the minimum waiting time as a delay to each attempt. If excessive events are tried to be sent (e.g. in a loop) a warning is displayed.
  <details><summary>Events overview (click for more details)</summary>

  | Events allowed every 1 second | Events allowed every 60 seconds |
  |---------------------------------| --------------------------------|
  | sendBrightnessEvent             | sendAirQualityEvent             |
  | sendChangeChannelEvent          | sendPowerSensorEvent            |
  | sendColorEvent                  | sendTemperatureEvent            |
  | sendColoColorTemperatureEvent   |                                 |
  | sendContactEvent                |                                 |
  | sendDoorbellEvent               |                                 |
  | sendDoorStateEvent              |                                 |
  | sendBandsEvent                  |                                 |
  | sendSelectInputEvent            |                                 |
  | sendLockStateEvent              |                                 |
  | sendModeEvent                   |                                 |
  | sendMediaControlEvent           |                                 |
  | sendMotionEvent                 |                                 |
  | sendMuteEvent                   |                                 |
  | sendSetPercentageEvent          |                                 |
  | sendPowerLevelEvent             |                                 |
  | sendPowerStateEvent             |                                 |
  | sendRangeValueEvent             |                                 |
  | sendTargetTemperatureEvent      |                                 |
  | sendThermostatModeEvent         |                                 |
  | sendToggleStateEvent            |                                 |
  | sendVolumeEvent                 |                                 |
  </details>

### Code cleanup
- Moved callback definitions outside from class definitions\
(possible by the use of a dedicated namespace)
- Static-Cast (CRTP) unified to Pointer.

## Version 2.9.5
- Fixed: SinricProPowerSensor was missing PowerStateController
- Improved: Improved error message when a callback is not implemented

## Version 2.9.4
- Fixed: Compiler error on libb64/cencode.h when ESP8266WebServer is included before SinricPro.h.

## Version 2.9.3
- RangeController accepts callbacks for integer and floating point values
- Event limitation takes into account different instances for generic controllers 
- SettingController implemented for future use

## Version 2.9.2
- Removed external crypto libraries
- Changed & fixed signature calculation
- use std::bind instead lambda functions for member callbacks (SinricProWesockets.h)

## Version 2.9.1
Bugfix
- SinricProTemperatureSensor (fixed wrong include)
- SinricProWindowAC (fixed copy & paste error)
- SinricProDevice (getDeviceId was accedently proteced)

## Version 2.9.0
New:
- support custom devices
- check for minimum websockets version

Fixed:
- MultiRelays example

## Version 2.8.1
- New Example: MultiRelays_advance (see examples/Relay)
- Moved existing relay example to new folder (examples/Relay)

## Version 2.8.0
- SSL is now enabled by default

## Version 2.7.6
- Internal changes
  removed QueueList.h and use std::queue instead

## Version 2.7.5
- New Examples 
  - Examples / Light / RGB_LED_Stripe_5050
  - Examples / Switch / Relay

## Version 2.7.4
- New
  - Speaker & TV `onAdjustVolume` callback changed:
    The new format is `bool onAdjustVolume(const String &deviceId, int &volumeDelta, bool volumeDefault);`
    `volumeDefault`: `false` if the user specified the amount by which to change the volume; otherwise `true`

## Version 2.7.3
Fixed
- `.c_str()` for AppKey, AppSecret and DeviceId return now a valid `const char*`

## Version 2.7.2
Fixed:
- The examples are now located in subfolders if more than one example is provided

## Version 2.7.1
Fixed:
- Switch example
  `SinricProDevice.getDeviceId()` returns `SinricProId` instead of `char*` since version 2.7.0

## Version 2.7.0
New:
- Added new classes for AppKey, AppSecret and DeviceId
  This allows to store credentials efficiently in EEPROM and reduces the internal RAM usage
  
- Added function to measure the RTT between device and SinricPro Server (SinricPro.onPong())

Fixed:
- Type mismatches in Crypto.h / Crypto.cpp (to reduce compiler warnings)

## Version 2.6.1
Fixed:
  Air Quality Sensor example for sensor type GP2Y1014AU0F

## Version 2.6.0
New:
- Devices
  - AirQualitySensor
	
- Examples
  - AirQualitySensor

Changed:
  - SinricProCamera device type added to support Camera. Camera example updated

## Version 2.5.0
New:
- Devices
  - Camera

- Examples
  -  Camera

## Version 2.4.0
New:
- Devices
  - PowerSensor

- Examples
  -  PowerSensor

## Version 2.3.0
New:
- Devices
  - Blinds
  - GarageDoor

- Examples
  - Thermostat
  - Blinds
  - GarageDoor

## Version 2.2.7

New:
- Examples
  - Dimmable Switch
  - Light
  - Speaker

Updated:
- User documentation
  - added SinricPro.onConnected() / SinricPro.onDisconnected()

Fixed:
- Examples
  - Some examples didn't use BAUD_RATE

## Version 2.2.6

New:
- User Documentation using Doxygen available at https://sinricpro.github.io/esp8266-esp32-sdk

Fixed: 
- Some sendEvent functions didn't return `true`/`false`
- Arduino TV example

## Version 2.2.5

New:
- Added new `callback` for TV `onChangeChannelNumber`
  This is to support changing channels using a
  channel number instead of channel name.
- New TV Example
- Added this `changelog.md`

Fixed: 
- Some sendEvent functions didn't return `true`/`false`
- Fixed json in `skipChannels` response
- Fixed json in `sendChangeChannels` event

Updated Readme
- about return values of sendEvent functions
  
---

## Version 2.2.4 

New:
- Added example for `SinricProTemperaturesensor`

Bugfix:
- Fixed typo in `onTargetTemperature` event

---

## Version 2.2.3

New :
- Added GarageDoorExample (ArudinoIDE) including wiring diagram
  This is based on `SinricProLock` device

Code optimizations:
- Changed callbacks to use `const String&` in callback functions
  to avoid unnecessary string copying
- Changed event limitiation to prevent server flooding

---

## Version 2.2.2

New Feature:
- `SinricPro.setResponseMessage(String &&message)`  allows to return a meaningful error message if callback returned false. 
  This message will shown in a red box on SinricPro portal website.  

- SinricProLock return state "JAMMED" now if callback failed (returned false)

Code optimization
- SinricProDevice is now default handler for powerState event/request

- SinricProSwitch is now a typedef to SinricProDevice since SinricProDevice handles onPowerState

- removed unneccessary includes

Bug fixes
- Fixed json protocol on events
- Fixed URL in library.properties
- Fixed UDP

---

## Version 2.2.1

New features:

- New Callback functions for onConnected / onDisconnected
  - SinricPro.onConnected(std::function<void(void)>);
  - SinricPro.onDisconnected(std::function<void(void)>);

- Automatic device addition
  - You can use ```SinricProSwitch& mySwitch = SinricPro[DEVICEID];``` to add a new device.

Changed:
- Stronger id validation (app-key, app-secret and deviceIds)
- Added more details to debug log about wrong id's

Fixed:
- Typos

---

## Version 2.2.0

New features:
- Removed NTPClient dependency
- Extract timeStamp from messages
  Keep timestamp in sync without NTP
- RestoreDeviceStates
  Request last known device states from server

Changed:
- Processessing of incomming messages
  Prepared for client-sided requests and server-sided responses
- Processing of outgoing messages
  Messages stored without signature / timestamp in sendQueue
  Timestamp and signature will applied while sendQueue is processed

---

## Version 2.1.1

Limited temperature and humidity to one decimal point

---

## Version 2.1.0

New interfaces introduced:
- SinricProInterface
- SinricProDeviceInterface

Marked deprecated functions:
- add(SinricProDeviceInterface& newDevice);
- add(SinricProDeviceInterface* newDevice);

---

## Version 2.0.5

fixed UDP problems on ESP32

---

## Version 2.0.4

**SinricPro.add()**
added deviceId validation check to avoid wrong deviceId's

---
## Version 2.0.2

**Devices**: Thermostat & WindowAC
fixed "adjustTargetTemperature" request

**SinricProWebsocket.h**
added server-address on websocket debug output

---
