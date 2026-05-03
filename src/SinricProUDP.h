/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#if defined ESP8266
  #include <ESP8266WiFi.h>
#endif
#if defined ESP32
  #include <WiFi.h>
#endif

#include <WiFiUdp.h>
#include "SinricProQueue.h"
#include "SinricProConfig.h"
#include "SinricProDebug.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

class UdpListener {
  public:
    void              begin(SinricProQueue_t* receiveQueue);
    void              handle();
    void              sendMessage(String &message);
    void              stop();

  private:
    WiFiUDP           _udp;
#if defined ESP8266
    WiFiUDP           _udpTx;
    bool              _txBegun = false;
#endif
    SinricProQueue_t* receiveQueue;
    IPAddress         _remoteIP;
    uint16_t          _remotePort = 0;
};

void UdpListener::begin(SinricProQueue_t* receiveQueue) {
  this->receiveQueue = receiveQueue;
  #if defined ESP8266
    _udp.beginMulticast(WiFi.localIP(), UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif
  #if defined ESP32
    _udp.beginMulticast(UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif
}

void UdpListener::handle() {
  int len = _udp.parsePacket();
  if (!len) return;

  _remoteIP   = _udp.remoteIP();
  _remotePort = _udp.remotePort();

  char* buf = (char*) malloc(len + 1);
  if (!buf) return;
  memset(buf, 0, len + 1);
  _udp.read(buf, len);
  SinricProMessage* request = new SinricProMessage(IF_UDP, buf);
  DEBUG_SINRIC("[SinricPro:UDP]: receiving request\r\n%s\r\n", buf);
  free(buf);
  receiveQueue->push(request);
}

void UdpListener::sendMessage(String &message) {
#if defined ESP8266
  _udpTx.beginPacket(_remoteIP, _remotePort);
  _udpTx.print(message);
  _udpTx.endPacket();
#endif

#if defined ESP32
  _udp.beginPacket(_remoteIP, _remotePort);
  _udp.print(message);
  _udp.endPacket();
#endif
}

void UdpListener::stop() {
  _udp.stop();
#if defined ESP8266
  _udpTx.stop();
  _txBegun = false;
#endif
}

} // SINRICPRO_NAMESPACE
