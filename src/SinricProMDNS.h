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
  #define SINRICPRO_MDNS_LEA
#elif defined(ESP32)
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #define SINRICPRO_MDNS_ESP32
#elif defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_ARCH_MBED)
  // arduino-pico ships LEAmDNS as a ESP8266mDNS replacement.
  #include <WiFi.h>
  #include <ESP8266mDNS.h>
  #define SINRICPRO_MDNS_LEA
#endif

#if defined(SINRICPRO_MDNS_LEA) || defined(SINRICPRO_MDNS_ESP32)
  #define SINRICPRO_MDNS_AVAILABLE
#endif

// Retry interval for starting the responder when it was not up yet
#ifndef SINRICPRO_MDNS_RETRY_MS
#define SINRICPRO_MDNS_RETRY_MS 5000
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
 *   deviceIds  — comma-separated list of registered device IDs
 *   sdk        — SDK version string (e.g. "5.0.0")
 *   udp        — always "1"
 *
 * Gate with SINRICPRO_NOMDNS to remove entirely.
 *
 * Call begin() once after WiFi is connected.  Call update() whenever the
 * device list changes; update() is cheap and idempotent, so it is safe to
 * call from a code path that runs on every loop().
 */
class SinricProMDNS {
  public:
    void begin(const String& hostName, const String& deviceIds);
    void update(const String& deviceIds);
    void handle();
  private:
    bool          _started    = false;
    bool          _announced  = false;
    unsigned long _lastStartTry = 0;
    String        _hostName;
    String        _deviceIds;

    bool start();
    void announce();
};

bool SinricProMDNS::start() {
#ifdef SINRICPRO_MDNS_AVAILABLE
    _lastStartTry = millis();
    _started      = MDNS.begin(_hostName.c_str());
    if (!_started) {
        DEBUG_SINRIC("[SinricPro:mDNS]: responder not started yet, will retry\r\n");
    }
#endif
    return _started;
}

void SinricProMDNS::announce() {
#ifdef SINRICPRO_MDNS_AVAILABLE
    if (!_announced) {
        MDNS.addService("sinricpro", "udp", UDP_MULTICAST_PORT);
        MDNS.addServiceTxt("sinricpro", "udp", "sdk", SINRICPRO_VERSION);
        MDNS.addServiceTxt("sinricpro", "udp", "udp", "1");
        _announced = true;
    }

    MDNS.addServiceTxt("sinricpro", "udp", "deviceIds", _deviceIds.c_str());
    DEBUG_SINRIC("[SinricPro:mDNS]: announced _sinricpro._udp.local. port=%d deviceIds=%s\r\n",
                 UDP_MULTICAST_PORT, _deviceIds.c_str());
#endif
}

void SinricProMDNS::begin(const String& hostName, const String& deviceIds) {
    _hostName  = hostName;
    _deviceIds = deviceIds;
    if (start()) announce();
}

void SinricProMDNS::update(const String& deviceIds) {
    if (_announced && _deviceIds == deviceIds) return;
    _deviceIds = deviceIds;
    if (_started) announce();
}

void SinricProMDNS::handle() {
#ifdef SINRICPRO_MDNS_AVAILABLE
    if (!_started) {
        if (millis() - _lastStartTry < SINRICPRO_MDNS_RETRY_MS) return;
        if (start()) announce();
        return;
    }
#ifdef SINRICPRO_MDNS_LEA
    MDNS.update();
#endif
    // On ESP32 the mDNS stack runs in a FreeRTOS task; no polling needed.
#endif
}

} // SINRICPRO_NAMESPACE

#endif // SINRICPRO_MDNS_ENABLED
