/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProConfig.h"

#ifdef SINRICPRO_MDNS_ENABLED

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <ESPmDNS.h>
#endif

#include "SinricProVersion.h"
#include "SinricProNamespace.h"
#include "SinricProDebug.h"

namespace SINRICPRO_NAMESPACE {

/**
 * @brief Announces this device on the LAN via mDNS.
 *
 * Service: _sinricpro._udp.local.  on UDP_MULTICAST_PORT.
 * TXT records:
 *   deviceIds  — semicolon-separated list of registered device IDs
 *   sdk        — SDK version string (e.g. "4.2.0")
 *   udp        — always "1"
 *
 * Gate with SINRICPRO_NOMDNS to remove entirely.
 *
 * Call begin() once after WiFi is connected and MDNS.begin() has been called
 * by user code (or it will be called internally here).
 * Call update() whenever the device list changes (e.g. after adding a device).
 */
class SinricProMDNS {
  public:
    void begin(const String& hostName, const String& deviceIds);
    void update(const String& deviceIds);
    void handle();
  private:
    bool _announced = false;
    void announce(const String& deviceIds);
};

void SinricProMDNS::begin(const String& hostName, const String& deviceIds) {
#if defined(ESP8266)
    // On ESP8266, MDNS.begin() must be called with the desired hostname.
    // If user code already called it, calling again is harmless.
    MDNS.begin(hostName.c_str());
#elif defined(ESP32)
    MDNS.begin(hostName.c_str());
#endif
    announce(deviceIds);
}

void SinricProMDNS::announce(const String& deviceIds) {
    // Remove any existing SinricPro service record before re-adding so that
    // the TXT record reflects the current device list.
    if (_announced) {
#if defined(ESP8266)
        // ESP8266mDNS 3.x: removeService(instance, service, protocol)
        // Passing nullptr as instance removes the default (unnamed) instance.
        MDNS.removeService(nullptr, "sinricpro", "udp");
#elif defined(ESP32)
        // ESPmDNS on ESP32 does not expose removeService; restart the responder.
        // The service port and hostname are small so this is fast.
        MDNS.end();
        MDNS.begin(WiFi.getHostname());
#endif
    }

    MDNS.addService("sinricpro", "udp", UDP_MULTICAST_PORT);
    MDNS.addServiceTxt("sinricpro", "udp", "deviceIds", deviceIds.c_str());
    MDNS.addServiceTxt("sinricpro", "udp", "sdk",       SINRICPRO_VERSION);
    MDNS.addServiceTxt("sinricpro", "udp", "udp",       "1");
    _announced = true;
    DEBUG_SINRIC("[SinricPro:mDNS]: announced _sinricpro._udp.local. port=%d deviceIds=%s\r\n",
                 UDP_MULTICAST_PORT, deviceIds.c_str());
}

void SinricProMDNS::update(const String& deviceIds) {
    announce(deviceIds);
}

void SinricProMDNS::handle() {
#if defined(ESP8266)
    MDNS.update();
#endif
    // On ESP32 the mDNS stack runs in a FreeRTOS task; no polling needed.
}

} // SINRICPRO_NAMESPACE

#endif // SINRICPRO_MDNS_ENABLED
