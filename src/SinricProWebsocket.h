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

//#include <WebSockets.h>
#include <WebSocketsClient.h>

#include <ArduinoJson.h>
#include "SinricProDebug.h"
#include "SinricProConfig.h"
#include "SinricProQueue.h"
#include "SinricProInterface.h"


#if !defined(WEBSOCKETS_VERSION_INT) || (WEBSOCKETS_VERSION_INT < 2003005)
#error "Wrong WebSockets Version! Minimum Version is 2.3.5!!!"
#endif

class AdvWebSocketsClient : public WebSocketsClient {
  public:
    void onPong(std::function<void(uint32_t)> cb) { _rttCb = cb; }
  protected:
    void messageReceived(WSclient_t * client, WSopcode_t opcode, uint8_t * payload, size_t length, bool fin) {
      if ((opcode == WSop_pong)&& (_rttCb)) {
        _rttCb(millis()-_client.lastPing);
      }
      WebSocketsClient::messageReceived(client, opcode, payload, length, fin);
    }
  private:
    std::function<void(uint32_t)> _rttCb = nullptr;
};

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
    void onPong(std::function<void(uint32_t)> cb) { webSocket.onPong(cb); }

    void disconnect() { webSocket.disconnect(); }
  private:
    bool _begin = false;
    bool _isConnected = false;
    bool restoreDeviceStates = false;

    AdvWebSocketsClient webSocket;

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
         headers += "ip:" + WiFi.localIP().toString() + "\r\n";
         headers += "mac:" + WiFi.macAddress() + "\r\n";
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
  webSocket.onEvent(std::bind(&websocketListener::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  webSocket.enableHeartbeat(WEBSOCKET_PING_INTERVAL, WEBSOCKET_PING_TIMEOUT, WEBSOCKET_RETRY_COUNT);
#ifdef WEBSOCKET_SSL
  webSocket.beginSSL(server.c_str(), SINRICPRO_SERVER_SSL_PORT, "/");
#else
  webSocket.begin(server.c_str(), SINRICPRO_SERVER_PORT, "/"); // server address, port and URL
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
  (void) length;
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
