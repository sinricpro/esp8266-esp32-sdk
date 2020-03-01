# Changelog

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
