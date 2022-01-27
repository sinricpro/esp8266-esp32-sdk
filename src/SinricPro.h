/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include <WebSocketsClient.h>

#include "SinricProDeviceInterface.h"
#include "SinricProInterface.h"
#include "SinricProMessageid.h"
#include "SinricProNamespace.h"
#include "SinricProQueue.h"
#include "SinricProSignature.h"
#include "SinricProStrings.h"
#include "SinricProUDP.h"

namespace SINRICPRO_NAMESPACE {

enum ConnectionState {
    CS_UNINITALIZED,
    CS_DISCONNECTED,
    CS_CONNECTING,
    CS_CONNECTED,
    CS_STOP
};

enum DisconnectReason {
    DR_WIFI      = -1,
    DR_WEBSOCKET = -2,
    DR_UNKNOWN   = -3
};

/**
 * @brief Callback definition for onConnected function
 * 
 * Gets called when device is connected to SinricPro server
 * @param void
 * @return void
 */
using ConnectedCallbackHandler = std::function<void(void)>;

/**
 * @brief Callback definition for onDisconnected function
 * 
 * Gets called when device is disconnected from SinricPro server
 * @param void
 * @return void
 */
using DisconnectedCallbackHandler = std::function<void(int)>;

/**
 * @class SinricProClass
 * @ingroup SinricPro
 * @brief The main class of this library, handling communication between SinricPro Server and your devices
 **/
class SinricProClass : public SinricProInterface, protected WebSocketsClient {
    friend class SinricProDevice;

  public:
    void begin(String appKey, String appSecret, String serverURL = SINRICPRO_SERVER_URL);
    void handle();
    void stop();
    bool isConnected() override;

    void onConnected(ConnectedCallbackHandler cb);
    void onDisconnected(DisconnectedCallbackHandler cb);

    void restoreDeviceStates(bool flag);
    void setResponseMessage(String&& message);

    unsigned long getTimestamp() override;

    struct proxy {
        proxy(SinricProClass* ptr, String deviceId)
            : ptr(ptr), deviceId(deviceId) {}
        SinricProClass* ptr;
        String          deviceId;
        template <typename T>
        operator T&() { return ptr->getDeviceInstance<T>(deviceId); }
    };

    proxy operator[](const String deviceId);

  protected:
    void registerDevice(SinricProDeviceInterface* device);
    void unregisterDevice(SinricProDeviceInterface* device);

    DynamicJsonDocument prepareResponse(JsonDocument& requestMessage);
    DynamicJsonDocument prepareEvent(String deviceId, const char* action, const char* cause) override;

    void sendMessage(JsonDocument& jsonMessage) override;

    void handleReceiveQueue();
    void handleSendQueue();

    void handleRequest(DynamicJsonDocument& requestMessage, interface_t Interface);
    void handleResponse(DynamicJsonDocument& responseMessage);

    DynamicJsonDocument prepareRequest(String deviceId, const char* action);

    void connect();
    void extractTimestamp(JsonDocument& message);
    void setExtraHeaders();

    SinricProDeviceInterface* getDevice(String deviceId);
    template <typename T>
    T& getDeviceInstance(String deviceId);

    virtual void runCbEvent(WStype_t type, uint8_t* payload, size_t length) override;

    std::vector<SinricProDeviceInterface*> devices;

    String appKey;
    String appSecret;
    String serverURL;

    UdpListener      _udpListener;
    SinricProQueue_t receiveQueue;
    SinricProQueue_t sendQueue;

    unsigned long baseTimestamp = 0;

    bool   _restoreDeviceStates = false;
    String responseMessageStr   = "";

    ConnectedCallbackHandler    _connectedCb    = nullptr;
    DisconnectedCallbackHandler _disconnectedCb = nullptr;

    ConnectionState _connection_state = CS_UNINITALIZED;
};

SinricProDeviceInterface* SinricProClass::getDevice(String deviceId) {
    for (auto& device : devices) {
        if (deviceId == device->getDeviceId()) return device;
    }
    return nullptr;
}

template <typename T>
T& SinricProClass::getDeviceInstance(String deviceId) {
    T* device = static_cast<T*>(getDevice(deviceId));

    if (!device) {
        DEBUG_SINRIC("[SinricPro]: Device with id \"%s\" does not exist in SDK yet. Creating new device\r\n", deviceId.c_str());
        device = new T(deviceId);
    }

    return *device;
}

/**
 * @brief Initializing SinricProClass to be able to connect to SinricPro Server
 * 
 * @param appKey `String` containing APP_KEY (see credentials from https://sinric.pro )
 * @param appSecret `String` containing APP_SECRET (see credentials from https:://sinric.pro)
 * @param serverURL `String` containing SinricPro Server URL (default="ws.sinric.pro")
 * @section Example-Code
 * @code
 * #define APP_KEY           "YOUR-APP-KEY"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
 * #define APP_SECRET        "YOUR-APP-SECRET"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
 * 
 * void setup() {
 *   SinricPro.begin(APP_KEY, APP_SECRET);
 * }
 * @endcode
 **/
void SinricProClass::begin(String appKey, String appSecret, String serverURL) {
    bool success = true;
    if (!appKey.length()) {
        DEBUG_SINRIC("[SinricPro]: App-Key \"%s\" is invalid!! Please check your app-key!! SinricPro will not work!\r\n", appKey.c_str());
        success = false;
    }
    if (!appSecret.length()) {
        DEBUG_SINRIC("[SinricPro]: App-Secret \"%s\" is invalid!! Please check your app-secret!! SinricPro will not work!\r\n", appSecret.c_str());
        success = false;
    }

    if (!success) return;

    if (_connection_state > CS_DISCONNECTED) {
        WebSocketsClient::disconnect();
    }
    _connection_state = CS_DISCONNECTED;

    this->appKey    = appKey;
    this->appSecret = appSecret;
    this->serverURL = serverURL;
    //    connect();
    _udpListener.begin(&receiveQueue);
}

void SinricProClass::registerDevice(SinricProDeviceInterface* device) {
    devices.push_back(device);
    DEBUG_SINRIC("[SinricPro]: The device with the ID \"%s\" has been registered.\r\n", device->getDeviceId().c_str());
}

void SinricProClass::unregisterDevice(SinricProDeviceInterface* device) {
    devices.erase(std::remove(devices.begin(), devices.end(), device), devices.end());
    DEBUG_SINRIC("[SinricPro]: The device with the ID \"%s\" has been unregistered.\r\n", device->getDeviceId().c_str());
}

/**
 * @brief Handles communication between device and SinricPro Server
 * 
 * This is the absolute main function which handles communication between your device and SinricPro Server. \n
 * It is responsible for connect, disconnect to SinricPro Server, handling requests, responses and events. \n
 * This function has to be called as often as possible. So it must be called in your main loop() function! \n
 * 
 * For proper function, begin() must be called with valid values for 'APP_KEY' and 'APP_SECRET' \n
 * @section handle Example-Code
 * @code
 * void loop() {
 *   SinricPro.handle();
 * }
 * @endcode
 **/
void SinricProClass::handle() {
    static bool begin_error = false;

    if (_connection_state == CS_UNINITALIZED) {
        if (!begin_error) {  // print this only once!
            DEBUG_SINRIC("[SinricPro]: ERROR! SinricPro.begin() failed or was not called prior to event handler\r\n");
            DEBUG_SINRIC("[SinricPro]:    -Reasons include an invalid app-key, invalid app-secret or no valid deviceIds)\r\n");
            DEBUG_SINRIC("[SinricPro]:    -SinricPro is disabled! Check earlier log messages for details.\r\n");
            begin_error = true;
        }
        return;
    }

    if (WiFi.isConnected()) {  // if we have wifi connection...

        if (_connection_state == CS_DISCONNECTED) connect();  // if we are not connected...initiate connect!
        WebSocketsClient::loop();                             // handle websocket
        _udpListener.handle();                                // handle udp

    } else {  // if we dont have a wifi connection

        if (_connection_state == CS_CONNECTED) {  // if we were connected before (wifi got lost...)
            DEBUG_SINRIC("[SinricPro]: Disconnected (reason: lost wifi)\r\n");
            if (_disconnectedCb) _disconnectedCb(DR_WIFI);  // call the disconnect callback..
            _connection_state = CS_DISCONNECTED;            // set state to disconnect (will start connecting in next round)
        }
    }

    handleReceiveQueue();
    handleSendQueue();

    for (auto& device : devices) device->loop();
}

DynamicJsonDocument SinricProClass::prepareRequest(String deviceId, const char* action) {
    DynamicJsonDocument requestMessage(1024);
    JsonObject          header              = requestMessage.createNestedObject(FSTR_SINRICPRO_header);
    header[FSTR_SINRICPRO_payloadVersion]   = 2;
    header[FSTR_SINRICPRO_signatureVersion] = 1;

    JsonObject payload                 = requestMessage.createNestedObject(FSTR_SINRICPRO_payload);
    payload[FSTR_SINRICPRO_action]     = action;
    payload[FSTR_SINRICPRO_createdAt]  = 0;
    payload[FSTR_SINRICPRO_deviceId]   = deviceId;
    payload[FSTR_SINRICPRO_replyToken] = MessageID().getID();
    payload[FSTR_SINRICPRO_type]       = FSTR_SINRICPRO_request;
    payload.createNestedObject(FSTR_SINRICPRO_value);
    return requestMessage;
}

void SinricProClass::handleResponse(DynamicJsonDocument& responseMessage) {
    (void)responseMessage;
    DEBUG_SINRIC("[SinricPro]: handling response\r\n");

#ifndef NODEBUG_SINRIC
    serializeJsonPretty(responseMessage, DEBUG_ESP_PORT);
    Serial.println();
#endif
}

void SinricProClass::handleRequest(DynamicJsonDocument& requestMessage, interface_t Interface) {
    DEBUG_SINRIC("[SinricPro]: handling request\r\n");
#ifndef NODEBUG_SINRIC
    serializeJsonPretty(requestMessage, DEBUG_ESP_PORT);
    Serial.println();
#endif

    DynamicJsonDocument responseMessage = prepareResponse(requestMessage);

    // handle devices
    bool        success        = false;
    const char* deviceId       = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_deviceId];
    String      action         = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action] | "";
    String      instance       = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId] | "";
    JsonObject  request_value  = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
    JsonObject  response_value = responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];

    for (auto& device : devices) {
        if (device->getDeviceId() == deviceId && success == false) {
            SinricProRequest request(action, instance, request_value, response_value);

            success = device->handleRequest(request);

            responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_success] = success;

            if (!success) {
                if (responseMessageStr.length()) {
                    responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_message] = responseMessageStr;

                    responseMessageStr = "";
                } else {
                    responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_message] = "Device did not handle \"" + action + "\"";
                }
            }
        }
    }

    String responseString;
    serializeJson(responseMessage, responseString);
    sendQueue.push(new SinricProMessage(Interface, responseString.c_str()));
}

void SinricProClass::handleReceiveQueue() {
    if (receiveQueue.size() == 0) return;

    DEBUG_SINRIC("[SinricPro]: %i message(s) in receiveQueue\r\n", receiveQueue.size());
    while (receiveQueue.size() > 0) {
        SinricProMessage* rawMessage = receiveQueue.front();
        receiveQueue.pop();
        DynamicJsonDocument jsonMessage(1024);
        deserializeJson(jsonMessage, rawMessage->getMessage());

        bool sigMatch = false;

        if (strncmp(rawMessage->getMessage(), "{\"timestamp\":", 13) == 0 && strlen(rawMessage->getMessage()) <= 26) {
            sigMatch = true;  // timestamp message has no signature...ignore sigMatch for this!
        } else {
            String signature           = jsonMessage[FSTR_SINRICPRO_signature][FSTR_SINRICPRO_HMAC] | "";
            String payload             = extractPayload(rawMessage->getMessage());
            String calculatedSignature = calculateSignature(appSecret.c_str(), payload);
            sigMatch                   = (calculatedSignature == signature);
        }

        String messageType = jsonMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_type];

        if (sigMatch) {  // signature is valid process message
            DEBUG_SINRIC("[SinricPro]: Signature is valid. Processing message...\r\n");
            extractTimestamp(jsonMessage);
            if (messageType == FSTR_SINRICPRO_response) handleResponse(jsonMessage);
            if (messageType == FSTR_SINRICPRO_request) handleRequest(jsonMessage, rawMessage->getInterface());
        } else {
            DEBUG_SINRIC("[SinricPro]: Signature is invalid! Sending messsage to [dev/null] ;)\r\n");
        }
        delete rawMessage;
    }
}

void SinricProClass::handleSendQueue() {
    if (!isConnected()) return;
    if (!baseTimestamp) return;
    while (sendQueue.size() > 0) {
        DEBUG_SINRIC("[SinricPro]: %i message(s) in sendQueue\r\n", sendQueue.size());
        DEBUG_SINRIC("[SinricPro]: Sending message...\r\n");

        SinricProMessage* rawMessage = sendQueue.front();
        sendQueue.pop();

        DynamicJsonDocument jsonMessage(1024);
        deserializeJson(jsonMessage, rawMessage->getMessage());
        jsonMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_createdAt] = getTimestamp();
        signMessage(appSecret, jsonMessage);

        String messageStr;

        serializeJson(jsonMessage, messageStr);
#ifndef NODEBUG_SINRIC
        serializeJsonPretty(jsonMessage, DEBUG_ESP_PORT);
        Serial.println();
#endif

        switch (rawMessage->getInterface()) {
            case IF_WEBSOCKET:
                DEBUG_SINRIC("[SinricPro]: Sending to websocket\r\n");
                WebSocketsClient::sendTXT(messageStr);
                break;
            case IF_UDP:
                DEBUG_SINRIC("[SinricPro]: Sending to UDP\r\n");
                _udpListener.sendMessage(messageStr);
                break;
            default:
                break;
        }
        delete rawMessage;
        DEBUG_SINRIC("[SinricPro]: message sent.\r\n");
    }
}

void SinricProClass::connect() {
    if (_connection_state != CS_DISCONNECTED) return;
    _connection_state = CS_CONNECTING;

#ifdef WEBSOCKET_SSL
    DEBUG_SINRIC("[SinricPro]: Connecting to WebSocket Server using SSL (%s)\r\n", serverURL.c_str());
#else
    DEBUG_SINRIC("[SinricPro]: Connecting to WebSocket Server (%s)\r\n", serverURL.c_str());
#endif

    setExtraHeaders();
    WebSocketsClient::enableHeartbeat(WEBSOCKET_PING_INTERVAL, WEBSOCKET_PING_TIMEOUT, WEBSOCKET_RETRY_COUNT);

#ifdef WEBSOCKET_SSL
    WebSocketsClient::beginSSL(serverURL.c_str(), SINRICPRO_SERVER_SSL_PORT, "/");
#else
    WebSocketsClient::begin(serverURL.c_str(), SINRICPRO_SERVER_PORT, "/");  // server address, port and URL
#endif
}

void SinricProClass::stop() {
    DEBUG_SINRIC("[SinricPro]: stopped\r\n");
    if (_connection_state > CS_UNINITALIZED) {
        WebSocketsClient::disconnect();
        _connection_state = CS_STOP;
    }
}

bool SinricProClass::isConnected() {
    return _connection_state == CS_CONNECTED;
}

/**
 * @brief Set callback function for websocket connected event
 * 
 * @param cb Function pointer to a `ConnectedCallbackHandler` function
 * @return void
 * @see ConnectedCallbackHandler
 * @section onConnected Example-Code
 * @snippet callbacks.cpp onConnected
 **/
void SinricProClass::onConnected(ConnectedCallbackHandler cb) {
    _connectedCb = cb;
}

/**
 * @brief Set callback function for websocket disconnected event
 * 
 * @param cb Function pointer to a `DisconnectedCallbackHandler` function
 * @return void
 * @see DisconnectedCallbackHandler
 * @section onDisconnected Example-Code
 * @snippet callbacks.cpp onDisconnected
 **/
void SinricProClass::onDisconnected(DisconnectedCallbackHandler cb) {
    _disconnectedCb = cb;
}

void SinricProClass::setExtraHeaders() {
    String device_list = "";
    int    i           = 0;
    for (auto& device : devices) {
        if (i++ > 0) device_list += ";";
        device_list += device->getDeviceId();
    }

    String headers = "appkey:" + appKey;
    headers += "\r\ndeviceids:" + device_list;
    headers += "\r\nrestoredevicestates:" + String(_restoreDeviceStates ? "true" : "false");
    headers += "\r\nip:" + WiFi.localIP().toString();
    headers += "\r\nmac:" + WiFi.macAddress();

#ifdef ESP8266
    headers += "\r\nplatform:ESP8266";
#endif

#ifdef ESP32
    headers += "\r\nplatform:ESP32";
#endif

    headers += "\r\nSDKversion:" + String(SINRICPRO_VERSION);

#ifdef FIRMWARE_VERSION
    headers += "\r\nfirmwareVersion:" + String(FIRMWARE_VERSION);
#endif

    DEBUG_SINRIC("[SinricPro]: headers: \r\n\r\n%s\r\n\r\n", headers.c_str());
    WebSocketsClient::setExtraHeaders(headers.c_str());

    // remove restoreDeviceStates for reconnections
    _restoreDeviceStates = false;
}

void SinricProClass::extractTimestamp(JsonDocument& message) {
    unsigned long tempTimestamp = 0;
    // extract timestamp from timestamp message right after websocket connection is established
    tempTimestamp = message["timestamp"] | 0;
    if (tempTimestamp) {
        baseTimestamp = tempTimestamp - (millis() / 1000);
        DEBUG_SINRIC("[SinricPro]: Got Timestamp %lu\r\n", tempTimestamp);
        return;
    }

    // update timestamp from request message
    tempTimestamp = message[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_createdAt] | 0;
    if (tempTimestamp) {
        baseTimestamp = tempTimestamp - (millis() / 1000);
        return;
    }
}

void SinricProClass::sendMessage(JsonDocument& jsonMessage) {
    if (!isConnected()) {
        DEBUG_SINRIC("[SinricPro]: device is offline, message has been dropped\r\n");
        return;
    }
    DEBUG_SINRIC("[SinricPro]: pushing message into sendQueue\r\n");
    String messageString;
    serializeJson(jsonMessage, messageString);
    sendQueue.push(new SinricProMessage(IF_WEBSOCKET, messageString.c_str()));
}

/**
 * @brief Enable / disable restore device states function
 * 
 * If this flag is enabled (`true`), SinricProServer will send last known device states to your device directly after connection to SinricPro server has been established. \n 
 * For every state the corresponding callback (like `onPowerState`) will be called \n
 * This is useful after a power failure / reboot of your device.
 * 
 * @param flag `true` = enabled \n `false`= disabled
 **/
void SinricProClass::restoreDeviceStates(bool flag) {
    _restoreDeviceStates = flag;
}

__attribute__((deprecated))
SinricProClass::proxy
SinricProClass::operator[](const String deviceId) {
    return proxy(this, deviceId);
}

void SinricProClass::setResponseMessage(String&& message) {
    responseMessageStr = message;
}

/**
 * @brief Get the current timestamp
 * 
 * @return unsigned long current timestamp (unix epoch time)
 */
unsigned long SinricProClass::getTimestamp() {
    return baseTimestamp + (millis() / 1000);
}

DynamicJsonDocument SinricProClass::prepareResponse(JsonDocument& requestMessage) {
    DynamicJsonDocument responseMessage(1024);
    JsonObject          header              = responseMessage.createNestedObject(FSTR_SINRICPRO_header);
    header[FSTR_SINRICPRO_payloadVersion]   = 2;
    header[FSTR_SINRICPRO_signatureVersion] = 1;

    JsonObject payload                = responseMessage.createNestedObject(FSTR_SINRICPRO_payload);
    payload[FSTR_SINRICPRO_action]    = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action];
    payload[FSTR_SINRICPRO_clientId]  = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_clientId];
    payload[FSTR_SINRICPRO_createdAt] = 0;
    payload[FSTR_SINRICPRO_deviceId]  = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_deviceId];
    if (requestMessage[FSTR_SINRICPRO_payload].containsKey(FSTR_SINRICPRO_instanceId)) payload[FSTR_SINRICPRO_instanceId] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId];
    payload[FSTR_SINRICPRO_message]    = FSTR_SINRICPRO_OK;
    payload[FSTR_SINRICPRO_replyToken] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_replyToken];
    payload[FSTR_SINRICPRO_success]    = false;
    payload[FSTR_SINRICPRO_type]       = FSTR_SINRICPRO_response;
    payload.createNestedObject(FSTR_SINRICPRO_value);
    return responseMessage;
}

DynamicJsonDocument SinricProClass::prepareEvent(String deviceId, const char* action, const char* cause) {
    DynamicJsonDocument eventMessage(1024);
    JsonObject          header              = eventMessage.createNestedObject(FSTR_SINRICPRO_header);
    header[FSTR_SINRICPRO_payloadVersion]   = 2;
    header[FSTR_SINRICPRO_signatureVersion] = 1;

    JsonObject payload             = eventMessage.createNestedObject(FSTR_SINRICPRO_payload);
    payload[FSTR_SINRICPRO_action] = action;
    payload[FSTR_SINRICPRO_cause].createNestedObject(FSTR_SINRICPRO_type);
    payload[FSTR_SINRICPRO_cause][FSTR_SINRICPRO_type] = cause;
    payload[FSTR_SINRICPRO_createdAt]                  = 0;
    payload[FSTR_SINRICPRO_deviceId]                   = deviceId;
    payload[FSTR_SINRICPRO_replyToken]                 = MessageID().getID();
    payload[FSTR_SINRICPRO_type]                       = FSTR_SINRICPRO_event;
    payload.createNestedObject(FSTR_SINRICPRO_value);
    return eventMessage;
}

void SinricProClass::runCbEvent(WStype_t type, uint8_t* payload, size_t length) {
    (void)length;

    switch (type) {
        case WStype_DISCONNECTED:
            if (_connection_state != CS_CONNECTED) break;
            DEBUG_SINRIC("[SinricPro]: Disconnected (reason: lost websocket connection)\r\n");
            if (_disconnectedCb) _disconnectedCb(DR_WEBSOCKET);
            _connection_state = CS_DISCONNECTED;
            break;

        case WStype_CONNECTED:
            DEBUG_SINRIC("[SinricPro]: connected\r\n");
            if (_connectedCb) _connectedCb();
            if (_restoreDeviceStates) {
                _restoreDeviceStates = false;
            }
            _connection_state = CS_CONNECTED;
            break;

        case WStype_TEXT: {
            SinricProMessage* request = new SinricProMessage(IF_WEBSOCKET, (char*)payload);
            DEBUG_SINRIC("[SinricPro]: receiving data\r\n");
            receiveQueue.push(request);
            break;
        }

        case WStype_PING: {
            DEBUG_SINRIC("[SinricPro]: websocket \"ping\" received\r\n");
            break;
        }

        case WStype_PONG: {
            unsigned long rtt = millis() - _client.lastPing;
            DEBUG_SINRIC("[SinricPro]: websocket \"pong\" received (%lu ms)\r\n", rtt);
            break;
        }

        default:
            break;
    }
}

}  // namespace SINRICPRO_NAMESPACE

SINRICPRO_NAMESPACE::SinricProClass SinricPro;