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
3. Create a new Project
4. Use the [Library Manager](https://docs.platformio.org/en/latest/librarymanager/) to add the library to your project or add `sinricpro/SinricPro` manually to your `lib_deps` in `platformio.ini`.

![sinricpro library manager](https://raw.githubusercontent.com/sinricpro/images/master/platformio-install-sinricpro.png)

### ArduinoIDE
1. Open Library Manager (*Tools / Manage Libraries*)  
2. Search for *SinricPro* and click *Install*  
3. Repeat step 2 for all [dependent libraries](#dependencies)!
4. Open example in ArduinoIDE (*File / Examples / SinricPro / ...*)  

![ArduinoIDE Library Manager](https://raw.githubusercontent.com/sinricpro/images/master/ArduinoIDE-Library-Manager.png)

---

## Dependencies
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
