/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #define SINRICPRO_UDP_LWIP
#elif defined(ESP32)
  #include <WiFi.h>
  #define SINRICPRO_UDP_ESP32
#elif defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
  #define SINRICPRO_UDP_LWIP
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
    void              sendMessage(String &message, const IPAddress& remoteIP, uint16_t remotePort);
    void              stop();

  private:
    WiFiUDP           _udp;
    SinricProQueue_t* receiveQueue;
};

void UdpListener::begin(SinricProQueue_t* receiveQueue) {
  this->receiveQueue = receiveQueue;

  // beginMulticast() leaves no socket listening if the IGMP join fails.
  uint8_t started = 0;

#if defined(SINRICPRO_UDP_LWIP)
  started = _udp.beginMulticast(WiFi.localIP(), UDP_MULTICAST_IP, UDP_MULTICAST_PORT);

#elif defined(SINRICPRO_UDP_ESP32)
  started = _udp.beginMulticast(UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
#endif

  if (started) {
    DEBUG_SINRIC("[SinricPro:UDP]: listening on port %d\r\n", UDP_MULTICAST_PORT);
  } else {
    DEBUG_SINRIC("[SinricPro:UDP]: could not listen on port %d, local control unavailable\r\n", UDP_MULTICAST_PORT);
  }
}

void UdpListener::handle() {
  int len = _udp.parsePacket();
  if (!len) return;

  char* buf = (char*) malloc(len + 1);
  if (!buf) return;
  memset(buf, 0, len + 1);
  _udp.read(buf, len);
  const IPAddress peerIP   = _udp.remoteIP();
  const uint16_t  peerPort = _udp.remotePort();
  SinricProMessage* request = new SinricProMessage(IF_UDP, buf, peerIP, peerPort);
  DEBUG_SINRIC("[SinricPro:UDP]: receiving request\r\n%s\r\n", buf);
  free(buf);
  receiveQueue->push(request);
}

void UdpListener::sendMessage(String &message, const IPAddress& remoteIP, uint16_t remotePort) {
  if (!remotePort) {
    DEBUG_SINRIC("[SinricPro:UDP]: message has no peer to answer, dropping\r\n");
    return;
  }
#if defined(SINRICPRO_UDP_LWIP) || defined(SINRICPRO_UDP_ESP32)
  // Reply on the listening socket. A separate send-only WiFiUDP does not
  // transmit on ESP8266 lwIP -- endPacket() returns success and sends nothing.
  const int begun = _udp.beginPacket(remoteIP, remotePort);
  _udp.print(message);
  const int sent = _udp.endPacket();

  if (!begun || !sent) {
    DEBUG_SINRIC("[SinricPro:UDP]: reply to %s failed (begin=%d send=%d)\r\n",
                 remoteIP.toString().c_str(), begun, sent);
  }
#else
  (void)message;
  (void)remoteIP;
#endif
}

void UdpListener::stop() {
  _udp.stop();
}

} // SINRICPRO_NAMESPACE
