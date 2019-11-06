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
    SinricProQueue_t* receiveQueue;

};

websocketListener::websocketListener() : _isConnected(false) {}

websocketListener::~websocketListener() {
  stop();
}

void websocketListener::begin(String server, String socketAuthToken, String deviceIds, SinricProQueue_t* receiveQueue) {
  if (_begin) return;
  _begin = true;
  this->receiveQueue = receiveQueue;
  DEBUG_SINRIC("[SinricPro:Websocket]: Conecting to WebSocket Server (%s)\r\n", server.c_str());

  if (_isConnected) {
    stop();
  }

  String headers = "appkey:" + socketAuthToken + "\r\n" + "deviceids:" + deviceIds + "\r\n";
  headers += "restoredevicestates:" + String(restoreDeviceStates?"true":"false") + "\r\n";
  #ifdef ESP8266
         headers += "platform:ESP8266\r\n";
  #endif
  #ifdef ESP32
         headers += "platform:ESP32\r\n";
  #endif
  headers += "version:" + String(SDK_VERSION);
  DEBUG_SINRIC("[SinricPro:Websocket]: headers: \r\n%s\r\n", headers.c_str());
  webSocket.setExtraHeaders(headers.c_str());
  webSocket.onEvent([&](WStype_t type, uint8_t * payload, size_t length) { webSocketEvent(type, payload, length); });
  webSocket.enableHeartbeat(WEBSOCKET_PING_INTERVAL, WEBSOCKET_PING_TIMEOUT, WEBSOCKET_RETRY_COUNT);
  webSocket.begin(server, SINRICPRO_SERVER_PORT, "/"); // server address, port and URL
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
