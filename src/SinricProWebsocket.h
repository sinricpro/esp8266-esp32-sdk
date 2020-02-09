/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */


#ifndef _SINRICPRO_WEBSOCKET_H__
#define _SINRICPRO_WEBSOCKET_H__

#if defined ESP8266
  #include <ESP8266WiFi.h>
#endif
#if defined ESP32
  #include <WiFi.h>
#endif

#ifdef WEBSOCKET_SSL
const char ENDPOINT_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow
SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT
GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC
AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF
q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8
SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0
Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA
a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj
/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T
AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG
CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv
bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k
c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw
VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC
ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz
MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu
Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF
AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo
uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/
wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu
X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG
PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6
KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==
-----END CERTIFICATE-----
)EOF";
#endif

#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "SinricProDebug.h"
#include "SinricProConfig.h"
#include "SinricProQueue.h"
#include "SinricProInterface.h"

class websocketListener
{
  public:
    typedef std::function<void(void)> wsConnectedCallback;
    typedef std::function<void(void)> wsDisconnectedCallback;

    websocketListener();
    ~websocketListener();

    void begin(String server, String socketAuthToken, String deviceIds, SinricProQueue_t* receiveQueue);
    void handle();
    void stop();
    bool isConnected() { return _isConnected; }
    void setRestoreDeviceStates(bool flag) { this->restoreDeviceStates = flag; };

    void sendMessage(String &message);

    void onConnected(wsConnectedCallback callback) { _wsConnectedCb = callback; }
    void onDisconnected(wsDisconnectedCallback callback) { _wsDisconnectedCb = callback; }

    void disconnect() { webSocket.disconnect(); }
  private:
    bool _begin = false;
    bool _isConnected = false;
    bool restoreDeviceStates = false;

    WebSocketsClient webSocket;

    wsConnectedCallback _wsConnectedCb;
    wsDisconnectedCallback _wsDisconnectedCb;

    void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
    void setExtraHeaders();
    SinricProQueue_t* receiveQueue;
    String deviceIds;
    String socketAuthToken;
};

void websocketListener::setExtraHeaders() {
  String headers  = "appkey:" + socketAuthToken + "\r\n";
         headers += "deviceids:" + deviceIds + "\r\n";
         headers += "restoredevicestates:" + String(restoreDeviceStates?"true":"false") + "\r\n";
  #ifdef ESP8266
         headers += "platform:ESP8266\r\n";
  #endif
  #ifdef ESP32
         headers += "platform:ESP32\r\n";
  #endif
         headers += "version:" + String(SINRICPRO_VERSION);
  DEBUG_SINRIC("[SinricPro:Websocket]: headers: \r\n%s\r\n", headers.c_str());
  webSocket.setExtraHeaders(headers.c_str());
}

websocketListener::websocketListener() : _isConnected(false) {}

websocketListener::~websocketListener() {
  stop();
}

void websocketListener::begin(String server, String socketAuthToken, String deviceIds, SinricProQueue_t* receiveQueue) {
  if (_begin) return;
  _begin = true;

  this->receiveQueue = receiveQueue;
  this->socketAuthToken = socketAuthToken;
  this->deviceIds = deviceIds;

#ifdef WEBSOCKET_SSL
  DEBUG_SINRIC("[SinricPro:Websocket]: Connecting to WebSocket Server using SSL (%s)\r\n", server.c_str());
#else
  DEBUG_SINRIC("[SinricPro:Websocket]: Connecting to WebSocket Server (%s)\r\n", server.c_str());
#endif

  if (_isConnected) {
    stop();
  }
  setExtraHeaders();
  webSocket.onEvent([&](WStype_t type, uint8_t * payload, size_t length) { webSocketEvent(type, payload, length); });
  webSocket.enableHeartbeat(WEBSOCKET_PING_INTERVAL, WEBSOCKET_PING_TIMEOUT, WEBSOCKET_RETRY_COUNT);
#ifdef WEBSOCKET_SSL
  webSocket.beginSslWithCA("testws.sinric.pro", 443, "/", ENDPOINT_CA_CERT);
#else
  webSocket.begin(server, SINRICPRO_SERVER_PORT, "/"); // server address, port and URL
#endif
}

void websocketListener::handle() {
  webSocket.loop();
}

void websocketListener::stop() {
  if (_isConnected) {
    webSocket.disconnect();
  }
  _begin = false;
}

void websocketListener::sendMessage(String &message) {
  webSocket.sendTXT(message);
}
 

void websocketListener::webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
  switch (type) {
    case WStype_DISCONNECTED:
      if (_isConnected) {
        DEBUG_SINRIC("[SinricPro:Websocket]: disconnected\r\n");
        if (_wsDisconnectedCb) _wsDisconnectedCb();
        _isConnected = false;
      }
      break;
    case WStype_CONNECTED:
      _isConnected = true;
      DEBUG_SINRIC("[SinricPro:Websocket]: connected\r\n");
      if (_wsConnectedCb) _wsConnectedCb();
      if (restoreDeviceStates) {
        restoreDeviceStates=false; 
        setExtraHeaders();
      }
      break;
    case WStype_TEXT: {
      SinricProMessage* request = new SinricProMessage(IF_WEBSOCKET, (char*)payload);
      DEBUG_SINRIC("[SinricPro:Websocket]: receiving data\r\n");
      receiveQueue->push(request);
      break;
    }
    default: break;
  }
}

#endif
