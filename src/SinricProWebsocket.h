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

#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "SinricProConfig.h"
#include "SinricProDebug.h"
#include "SinricProInterface.h"
#include "SinricProNamespace.h"
#include "SinricProQueue.h"
namespace SINRICPRO_NAMESPACE {

#if !defined(WEBSOCKETS_VERSION_INT) || (WEBSOCKETS_VERSION_INT < 2003005)
#error "Wrong WebSockets Version! Minimum Version is 2.3.5!!!"
#endif

using wsConnectedCallback    = std::function<void(void)>;
using wsDisconnectedCallback = std::function<void(void)>;
using wsPongCallback         = std::function<void(uint32_t)>;

class WebsocketListener : protected WebSocketsClient {
  public:
    WebsocketListener();
    ~WebsocketListener();

    void begin(String server, String appKey, String deviceIds, SinricProQueue_t* receiveQueue);
    void handle();
    void stop();
    void setRestoreDeviceStates(bool flag);

    void sendMessage(String& message);

    void onConnected(wsConnectedCallback callback);
    void onDisconnected(wsDisconnectedCallback callback);
    void onPong(wsPongCallback callback);
    
    using WebSocketsClient::disconnect;
    using WebSocketsClient::isConnected;

  protected:
    bool _begin;
    bool restoreDeviceStates;

    wsConnectedCallback    _wsConnectedCb;
    wsDisconnectedCallback _wsDisconnectedCb;
    wsPongCallback         _wsPongCb;

    virtual void runCbEvent(WStype_t type, uint8_t* payload, size_t length) override;

    void              setExtraHeaders();
    SinricProQueue_t* receiveQueue;
    String            deviceIds;
    String            appKey;
};

WebsocketListener::WebsocketListener()
    : _begin(false)
    , restoreDeviceStates(false)
    , _wsConnectedCb(nullptr)
    , _wsDisconnectedCb(nullptr)
    , _wsPongCb(nullptr) {}

WebsocketListener::~WebsocketListener() {
    stop();
}

void WebsocketListener::setExtraHeaders() {
#ifdef ESP8266
    const char* platform = "ESP8266";
#endif
#ifdef ESP32
    const char* platform = "ESP32";
#endif

    String headers = "appkey:" + appKey;
    headers += "\r\ndeviceids:" + deviceIds;
    headers += "\r\nrestoredevicestates:" + String(restoreDeviceStates ? "true" : "false");
    headers += "\r\nip:" + WiFi.localIP().toString();
    headers += "\r\nmac:" + WiFi.macAddress();
    headers += "\r\nplatform:" + String(platform);
    headers += "\r\nSDKVersion:" + String(SINRICPRO_VERSION);

#ifdef FIRMWARE_VERSION
    headers += "\r\nfirmwareVersion:" + String(FIRMWARE_VERSION);
#endif

    DEBUG_SINRIC("[SinricPro:Websocket]: headers: \r\n%s\r\n", headers.c_str());
    WebSocketsClient::setExtraHeaders(headers.c_str());
}

void WebsocketListener::begin(String server, String appKey, String deviceIds, SinricProQueue_t* receiveQueue) {
    if (_begin) return;
    _begin = true;

    this->receiveQueue = receiveQueue;
    this->appKey       = appKey;
    this->deviceIds    = deviceIds;

#ifdef WEBSOCKET_SSL
    DEBUG_SINRIC("[SinricPro:Websocket]: Connecting to WebSocket Server using SSL (%s)\r\n", server.c_str());
#else
    DEBUG_SINRIC("[SinricPro:Websocket]: Connecting to WebSocket Server (%s)\r\n", server.c_str());
#endif

    if (isConnected()) stop();
    setExtraHeaders();
    enableHeartbeat(WEBSOCKET_PING_INTERVAL, WEBSOCKET_PING_TIMEOUT, WEBSOCKET_RETRY_COUNT);
#ifdef WEBSOCKET_SSL
    WebSocketsClient::beginSSL(server.c_str(), SINRICPRO_SERVER_SSL_PORT, "/");
#else
    WebSocketsClient::begin(server.c_str(), SINRICPRO_SERVER_PORT, "/");  // server address, port and URL
#endif
}

void WebsocketListener::handle() {
    loop();
}

void WebsocketListener::stop() {
    disconnect();
    _begin = false;
}

void WebsocketListener::setRestoreDeviceStates(bool flag) {
    this->restoreDeviceStates = flag;
};

void WebsocketListener::sendMessage(String& message) {
    sendTXT(message);
}

void WebsocketListener::onConnected(wsConnectedCallback callback) {
    _wsConnectedCb = callback;
}

void WebsocketListener::onDisconnected(wsDisconnectedCallback callback) {
    _wsDisconnectedCb = callback;
}

void WebsocketListener::onPong(wsPongCallback callback) {
    _wsPongCb = callback;
}

void WebsocketListener::runCbEvent(WStype_t type, uint8_t* payload, size_t length) {
    (void)length;

    switch (type) {
        case WStype_DISCONNECTED: {
                DEBUG_SINRIC("[SinricPro:Websocket]: disconnected\r\n");
                if (_wsDisconnectedCb) _wsDisconnectedCb();
            }
            break;

        case WStype_CONNECTED:
            DEBUG_SINRIC("[SinricPro:Websocket]: connected\r\n");
            if (_wsConnectedCb) _wsConnectedCb();
            if (restoreDeviceStates) {
                restoreDeviceStates = false;
                setExtraHeaders();
            }
            break;

        case WStype_TEXT: {
            SinricProMessage* request = new SinricProMessage(IF_WEBSOCKET, (char*)payload);
            DEBUG_SINRIC("[SinricPro:Websocket]: receiving data\r\n");
            receiveQueue->push(request);
            break;
        }

        case WStype_PONG: {
            if (_wsPongCb) _wsPongCb(millis() - _client.lastPing);
            break;
        }

        default:
            break;
    }
}

}  // namespace SINRICPRO_NAMESPACE