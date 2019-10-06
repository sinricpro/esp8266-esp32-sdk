#ifndef __SINRIC_PRO_UDP_H__
#define __SINRIC_PRO_UDP_H__

#if defined ESP8266
  #include <ESP8266WiFi.h>
#endif
#if defined ESP32
  #include <WiFi.h>
#endif

#include <WiFiUdp.h>
#include "Globals.h"

class udpListener {
public:
  void begin();
  void handle();
  void sendResponse(String response);
  void stop();
private:
  WiFiUDP _udp;
};

void udpListener::begin() {
  #if defined ESP8266
    _udp.beginMulticast(WiFi.localIP(), UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif  
  #if defined ESP32
    _udp.beginMulticast(UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
  #endif  
}

void udpListener::handle() {
  int len = _udp.parsePacket();
  if (len) {

    char buffer[512];
    int n = _udp.read(buffer, 512);
    buffer[n] = 0;
    SinricProMessage* request = new SinricProMessage(IF_UDP, buffer);
    DEBUG_SINRIC("[SinricPro:UDP]: receiving request\r\n");
    receiveQueue.push(request);
  }
}

void udpListener::sendResponse(String response) {
  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.print(response);
//  _udp.write(response.c_str());
  _udp.endPacket();
}

void udpListener::stop() {
  _udp.stop();
}

#endif
