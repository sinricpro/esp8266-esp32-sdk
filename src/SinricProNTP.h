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
  unsigned long getTimestamp() { return _timeClient.getEpochTime(); }
private:
  WiFiUDP _udpClient;
  NTPClient _timeClient;
};

#endif // _TIMESTAMP_H_
