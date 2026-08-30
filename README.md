# SinricPro (ESP8266 / ESP32 / RP2040)
[![arduino-library-badge](https://www.ardu-badge.com/badge/SinricPro.svg?)](https://www.arduino.cc/reference/en/libraries/sinricpro) [![PlatformIO Registry](https://badges.registry.platformio.org/packages/sinricpro/library/SinricPro.svg)](https://registry.platformio.org/libraries/sinricpro/SinricPro)

[![Platform ESP8266](https://img.shields.io/badge/Platform-Espressif8266-orange)](#) [![Platform ESP32](https://img.shields.io/badge/Platform-Espressif32-orange)](#)
[![Raspberry Pi RP2040](https://img.shields.io/badge/Platform-Raspberry_Pi_RP2040-orange)](#)

[![Framework](https://img.shields.io/badge/Framework-Arduino-blue)](https://www.arduino.cc/)

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/sinricpro/esp8266-esp32-sdk)](https://github.com/sinricpro/esp8266-esp32-sdk/releases)

[![Build](https://github.com/sinricpro/esp8266-esp32-sdk/actions/workflows/build-esp8266-esp32.yml/badge.svg)](https://github.com/sinricpro/esp8266-esp32-sdk/actions/workflows/build-esp8266-esp32.yml)

[![Build](https://github.com/sinricpro/esp8266-esp32-sdk/actions/workflows/build-rpipicow.yml/badge.svg)](https://github.com/sinricpro/esp8266-esp32-sdk/actions/workflows/build-rpipicow.yml)

[![Discord](https://img.shields.io/badge/discord-%23esp8266--esp32-blue.svg)](https://discord.gg/rq9vcRcSqA) </br>
  
## Installation

### VS Code & PlatformIO:
1. Install [VS Code](https://code.visualstudio.com/)  
2. Install [PlatformIO](https://platformio.org/platformio-ide)  
3. Create a new Project and use the [Library Manager](https://docs.platformio.org/en/latest/librarymanager/) to add the library to your project or add `sinricpro/SinricPro` manually to your `lib_deps` in `platformio.ini`.
4. Complete `platformio.ini` is available [here](https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/examples/platformio/platformio.ini) 



![sinricpro library manager](https://raw.githubusercontent.com/sinricpro/images/master/platformio-install-sinricpro.png)



### ArduinoIDE
1. Open Library Manager (*Tools / Manage Libraries*)  
2. Search for *SinricPro* and click *Install*  
3. Repeat step 2 for all [dependent libraries](#dependencies)!
4. Open example in ArduinoIDE (*File / Examples / SinricPro / ...*)  

![ArduinoIDE Library Manager](https://raw.githubusercontent.com/sinricpro/images/master/ArduinoIDE-Library-Manager.png)

---

## Dependencies
- Arduino core 3.x
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) by Benoit Blanchon (minimum Version 7.0.3)
- [WebSockets](https://github.com/Links2004/arduinoWebSockets) by Markus Sattler (minimum Version 2.4.0)

---

## Full user documentation
Please see here for [full user documentation](https://sinricpro.github.io/esp8266-esp32-sdk-documentation/)

---

## Examples
See [examples](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples) on GitHub

---

## Usage
### Include SinricPro-Library (SinricPro.h) and SinricPro-Device-Libraries (eg. SinricProSwitch.h)
```C++
#include <SinricPro.h>
#include <SinricProSwitch.h>
```

### Define your credentials from SinricPro-Portal (portal.sinric.pro)
```C++
#define APP_KEY    "YOUR-APP-KEY"    // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "YOUR-APP-SECRET" // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define SWITCH_ID  "YOUR-DEVICE-ID"  // Should look like "5dc1564130xxxxxxxxxxxxxx"
```

### Define callback routine(s)
```C++
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("device %s turned %s\r\n", deviceId.c_str(), state?"on":"off");
  return true; // indicate that callback handled correctly
}
```

### In setup()
```C++
  // create and add a switch to SinricPro
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
  // set callback function
  mySwitch.onPowerState(onPowerState);
  // startup SinricPro
  SinricPro.begin(APP_KEY, APP_SECRET);

```

### In loop()
```C++
  SinricPro.handle();
```

---
## How to add a device?
Syntax is  
```C++
  DeviceType& myDevice = SinricPro[DEVICE_ID];
```
Example  
```C++
  SinricProSwitch& mySwitch = SinricPro["YOUR-SWITCH-ID-HERE"];
```

---
## How to retrieve a device for sending an event?
Syntax is  
```C++
  DeviceType& myDevice = SinricPro[DEVICE_ID];
```
Example 
```C++
  SinricProDoorbell& myDoorbell = SinricPro["YOUR-DOORBELL-ID-HERE"];
  myDoorbell.sendDoorbellEvent();
```

## How to send a push notification?
```C++
SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
mySwitch.sendPushNotification("Hello SinricPro!");
```

---

## Device Types
* [Switch](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Switch)
* [Dimmable Switch](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/DimSwitch)
* [Light](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Light)
* [TV](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/TV)
* [Speaker](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Speaker)
* [Thermostat](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Thermostat)
* [Fan](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Fan)
* [Lock](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Lock)
* [Doorbell](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/doorbell)
* [Temperature Sensor](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/temperaturesensor)
* [Motion Sensor](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/MotionSensor)
* [Contact Sensor](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/ContactSensor)
* [Windows Air Conditioner](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/ACUnit)
* [Blinds](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Blinds)
* [Garage Door](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/GarageDoor)
* [Custom devices](https://help.sinric.pro/pages/custom-templates)
* [Camera](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Camera)

### Other
* [OTA](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/OTAUpdate)
* [MultiWiFi](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Settings/MultiWiFi)
* [Health](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Health)
* [Settings](https://github.com/sinricpro/esp8266-esp32-sdk/tree/master/examples/Settings)
---

## Local Control (LAN/UDP)

Starting with SDK v5.0.0, SinricPro devices can be controlled directly on the
local network without a cloud round-trip.  When the mobile app cannot reach the
cloud within its timeout window, or when the user has selected "Local-first"
mode, the app sends a signed UDP command directly to the device IP address and
port.  The same HMAC-SHA256 credentials used for cloud communication are reused
for authentication — no separate provisioning step is needed.

### What it is

A UDP listener bound to the multicast group `224.9.9.9:3333` (also responds to
unicast to the device IP on the same port).  Incoming commands use exactly the
same JSON payload shape as cloud WebSocket commands and are dispatched through
the same capability callbacks (`onPowerState`, `onBrightness`, etc.).

### What changes for sketch authors

Nothing.  Existing callback registrations work for both cloud and LAN commands
automatically.  Local control is on by default.

### What changes for app developers

LAN availability is signaled implicitly: the app gates local-control on the
device reporting a non-empty `deviceIp` in the cloud model combined with an SDK
version >= 5.0.0.  No additional headers are required.  After a successful LAN
command the app posts the new state to `POST /api/v1/devices/:id/state` so the
cloud and other clients stay in sync — the firmware does not echo UDP responses
to the WebSocket.

### Compile-time flags

| Flag | Effect |
|---|---|
| `SINRICPRO_NOMDNS` | Disable the mDNS service announcement while keeping UDP active. |

Define this flag before including `SinricPro.h`, or pass it as a compiler
flag (`-DSINRICPRO_NOMDNS`) in `platformio.ini` / Arduino IDE build flags.

### Network requirements

- The phone and device must be on the same LAN segment (or a routed LAN that
  passes multicast and allows unicast to device IP:3333).
- UDP multicast is required only for mDNS-based discovery; unicast commands use
  the device IP reported by the cloud (`Device.lan.ip`).
- Port 3333 UDP must not be firewalled between the phone and the device.
- Enterprise or guest WiFi networks that block multicast/mDNS will fall back to
  the cloud-reported IP automatically.

### Security model

All UDP commands are signed with HMAC-SHA256 using the same `APP_SECRET` as
cloud commands.  The firmware verifies the signature before dispatching.

### mDNS service record

When `SINRICPRO_NOMDNS` is not defined the SDK announces:

```
Service type : _sinricpro._udp.local.
Port         : 3333
TXT records  : deviceIds=<comma-separated device IDs>
               sdk=<SDK version, e.g. "5.0.0">
               udp=1
```

Browse with `dns-sd -B _sinricpro._udp` (macOS) or
`avahi-browse -r _sinricpro._udp` (Linux) to discover devices.

---

## Licensing and Credits
* The Arduino IDE is developed and maintained by the Arduino team. The IDE is licensed under GPL.
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson) is licensed under the MIT.
* [WebSockets](https://github.com/Links2004/arduinoWebSockets) is licensed under the GNU LGPL.
* The [PlatformIO](https://github.com/platformio) is developed and maintained by the PlatformIO team. The Core is licensed under Apache License 2.0.

## Support for other boards
[https://github.com/sinricpro/arduino-variants-sdk](https://github.com/sinricpro/arduino-variants-sdk)

## Join the community!
Join us on our [Official Discord Server](https://discord.gg/rq9vcRcSqA)!
