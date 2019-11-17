/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef __SINRIC_PRO_UDP_H__
#define __SINRIC_PRO_UDP_H__

#if defined ESP8266
  #include <ESP8266WiFi.h>
#endif
#if defined ESP32
  #include <WiFi.h>
#endif

#include <WiFiUdp.h>
#include "SinricProQueue.h"

class udpListener {
public:
  void begin(SinricProQueue_t* receiveQueue);
  void handle();
  void sendMessage(String &message);
  void stop();
private:
  WiFiUDP _udp;
  SinricProQueue_t* receiveQueue;
};

void udpListener::begin(SinricProQueue_t* receiveQueue) {
  this->receiveQueue = receiveQueue;
  #if defined ESP8266
    _udp.beginMulticast(WiFi.localIP(), UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif  
  #if defined ESP32
    _udp.beginMulticast(UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif  
}

void udpListener::handle() {
  if (!_udp.available()) return;
  int len = _udp.parsePacket();
  if (len) {
    
    char buffer[1024];
    int n = _udp.read(buffer, 1024);
    buffer[n] = 0;
    SinricProMessage* request = new SinricProMessage(IF_UDP, buffer);
    DEBUG_SINRIC("[SinricPro:UDP]: receiving request\r\n");
    receiveQueue->push(request);
  }
}

void udpListener::sendMessage(String &message) {
  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.print(message);
  _udp.endPacket();
  // restart UDP??
  #if defined ESP8266
    _udp.beginMulticast(WiFi.localIP(), UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif  
  #if defined ESP32
    _udp.beginMulticast(UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif  
}

/*
void udpListener::sendMessage(String &message) {
  WiFiUDP UDPsender;
  UDPsender.beginPacket(_udp.remoteIP(), _udp.remotePort());
  UDPsender.print(message);
  UDPsender.endPacket();
}
*/ 

void udpListener::stop() {
  _udp.stop();
}

#endif
