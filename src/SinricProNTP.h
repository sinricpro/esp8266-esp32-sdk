/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _NTP_H_
#define _NTP_H_

#include <NTPClient.h>
#include <WiFiUdp.h>

#if defined ESP8266
  #include <ESP8266WiFi.h>
#endif
#if defined ESP32
  #include <WiFi.h>
#endif


class myNTP {
public:
  myNTP() : _timeClient(_udpClient) {}
  void begin() { _timeClient.begin(); _timeClient.update(); }
  void update() { _timeClient.update(); }
  unsigned long getTimestamp();
private:
  WiFiUDP _udpClient;
  NTPClient _timeClient;
};


unsigned long myNTP::getTimestamp() { 
  unsigned long timestamp;
  timestamp = _timeClient.getEpochTime();
  while (timestamp < 1572040800) {
    _timeClient.forceUpdate();
    timestamp = _timeClient.getEpochTime();
    yield();
  }
  return timestamp; 
}

#endif // _TIMESTAMP_H_
