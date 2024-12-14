/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProDeviceInterface.h"
#include "SinricProInterface.h"
#include "SinricProMessageid.h"
#include "SinricProModuleCommandHandler.h"
#include "SinricProNamespace.h"
#include "SinricProQueue.h"
#include "SinricProSignature.h"
#include "SinricProStrings.h"
#include "SinricProUDP.h"
#include "SinricProWebsocket.h"
#include "Timestamp.h"
namespace SINRICPRO_NAMESPACE {

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
using DisconnectedCallbackHandler = std::function<void(void)>;

/**
 * @brief Function signature for OTA update callback.
 *
 * This typedef defines a function pointer type for OTA (Over-The-Air) update callbacks.
 * The callback function should accept a URL string and a Version struct as parameters.
 *
 * @param url The URL from which the OTA update can be downloaded.
 * @param major The major version number
 * @param minor The minor version number
 * @param patch The patch version number
 * @param forceUpdate Skip the version check and apply the update.
 * @return bool True if the update process started successful, false otherwise.
 */
using OTAUpdateCallbackHandler = std::function<bool(const String& url, int major, int minor, int patch, bool enforce)>;

/**
 * @brief Function signature for setting a module setting.
 *
 * This callback is used to set a value for a specific setting identified by its ID.
 *
 * @param id The unique identifier of the setting to be set.
 * @param value The new value to be assigned to the setting.
 * @return bool Returns true if the setting was successfully updated, false otherwise.
 */
using SetSettingCallbackHandler = std::function<bool(const String& id, const String& value)>;

/**
 * @typedef ReportHealthCallbackHandler
 * @brief Defines a function type for reporting device health status.
 *
 * This typedef creates an alias for a std::function that takes a reference to a String
 * and returns a boolean value. It's designed to be used as a callback for health reporting
 *
 * @param healthReport A reference to a String that will contain the health status information.
 *                     The callback function should populate this string with relevant health data.
 *
 * @return bool Returns true if the health report was successfully handled
 *              false otherwise.
 */
using ReportHealthCallbackHandler = std::function<bool(String& healthReport)>;

using PongCallback = std::function<void(uint32_t)>;

/**
 * @class SinricProClass
 * @ingroup SinricPro
 * @brief The main class of this library, handling communication between SinricPro Server and your devices
 **/
class SinricProClass : public SinricProInterface {
    friend class SinricProDevice;

  public:
    class Proxy;

  public:
    void           begin(String appKey, String appSecret, String serverURL = SINRICPRO_SERVER_URL);
    void           handle();
    void           stop();
    bool           isConnected();
    void           onConnected(ConnectedCallbackHandler cb);
    void           onDisconnected(DisconnectedCallbackHandler cb);
    void           onPong(PongCallback cb);
    void           restoreDeviceStates(bool flag);
    void           setResponseMessage(String&& message);
    unsigned long  getTimestamp() override;
    virtual String sign(const String& message);
    Proxy          operator[](const String deviceId);
    void           onOTAUpdate(OTAUpdateCallbackHandler cb);
    void           onSetSetting(SetSettingCallbackHandler cb);
    void           onReportHealth(ReportHealthCallbackHandler cb);

  protected:
    template <typename DeviceType>
    DeviceType& add(String deviceId);

    void add(SinricProDeviceInterface& newDevice);
    void add(SinricProDeviceInterface* newDevice);

    JsonDocument prepareResponse(JsonDocument& requestMessage);
    JsonDocument prepareEvent(String deviceId, const char* action, const char* cause) override;
    void         sendMessage(JsonDocument& jsonMessage) override;

  private:
    void handleReceiveQueue();
    void handleSendQueue();

    void handleDeviceRequest(JsonDocument& requestMessage, interface_t Interface);
    void handleModuleRequest(JsonDocument& requestMessage, interface_t Interface);
    void handleResponse(JsonDocument& responseMessage);

    JsonDocument prepareRequest(String deviceId, const char* action);

    void connect();
    void disconnect();
    void reconnect();

    void onConnect();
    void onDisconnect();

    void extractTimestamp(JsonDocument& message);

    SinricProDeviceInterface* getDevice(String deviceId);

    template <typename DeviceType>
    DeviceType& getDeviceInstance(String deviceId);

    std::vector<SinricProDeviceInterface*> devices;

    String appKey;
    String appSecret;
    String serverURL;

    WebsocketListener _websocketListener;
    UdpListener       _udpListener;
    SinricProQueue_t  receiveQueue;
    SinricProQueue_t  sendQueue;

    Timestamp timestamp;

    bool   _begin             = false;
    String responseMessageStr = "";

    SinricProModuleCommandHandler _moduleCommandHandler;
};

class SinricProClass::Proxy {
  public:
    Proxy(SinricProClass* ptr, const String& deviceId);

    template <typename DeviceType>
    operator DeviceType&();

  protected:
    SinricProClass* ptr;
    String          deviceId;
};

SinricProClass::Proxy::Proxy(SinricProClass* ptr, const String& deviceId)
    : ptr(ptr), deviceId(deviceId) {}

template <typename DeviceType>
SinricProClass::Proxy::operator DeviceType&() {
    return ptr->getDeviceInstance<DeviceType>(deviceId);
}

SinricProDeviceInterface* SinricProClass::getDevice(String deviceId) {
    for (auto& device : devices) {
        if (deviceId == device->getDeviceId()) return device;
    }
    return nullptr;
}

template <typename DeviceType>
DeviceType& SinricProClass::getDeviceInstance(String deviceId) {
    DeviceType* tmp_device = (DeviceType*)getDevice(deviceId);
    if (tmp_device) return *tmp_device;

    DEBUG_SINRIC("[SinricPro]: Device \"%s\" does not exist in the internal device list. creating new device\r\n", deviceId.c_str());
    DeviceType& tmp_deviceInstance = add<DeviceType>(deviceId);

    if (isConnected()) {
        DEBUG_SINRIC("[SinricPro]: Reconnecting to server.\r\n");
        reconnect();
    }

    return tmp_deviceInstance;
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
        DEBUG_SINRIC("[SinricPro:begin()]: App-Key \"%s\" is invalid!! Please check your app-key!! SinricPro will not work!\r\n", appKey.c_str());
        success = false;
    }
    if (!appSecret.length()) {
        DEBUG_SINRIC("[SinricPro:begin()]: App-Secret \"%s\" is invalid!! Please check your app-secret!! SinricPro will not work!\r\n", appSecret.c_str());
        success = false;
    }

    if (!success) {
        _begin = false;
        return;
    }

    this->appKey    = appKey;
    this->appSecret = appSecret;
    this->serverURL = serverURL;
    _begin          = true;
    _udpListener.begin(&receiveQueue);
}

template <typename DeviceType>
DeviceType& SinricProClass::add(String deviceId) {
    DeviceType* newDevice = new DeviceType(deviceId);
    DEBUG_SINRIC("[SinricPro:add()]: Adding device with id \"%s\".\r\n", deviceId.c_str());
    newDevice->begin(this);

    devices.push_back(newDevice);
    return *newDevice;
}

__attribute__((deprecated("Please use DeviceType& myDevice = SinricPro.add<DeviceType>(String);"))) void SinricProClass::add(SinricProDeviceInterface* newDevice) {
    newDevice->begin(this);
    devices.push_back(newDevice);
}

__attribute__((deprecated("Please use DeviceType& myDevice = SinricPro.add<DeviceType>(String);"))) void SinricProClass::add(SinricProDeviceInterface& newDevice) {
    newDevice.begin(this);
    devices.push_back(&newDevice);
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
    if (!_begin) {
        if (!begin_error) {  // print this only once!
            DEBUG_SINRIC("[SinricPro:handle()]: ERROR! SinricPro.begin() failed or was not called prior to event handler\r\n");
            DEBUG_SINRIC("[SinricPro:handle()]:    -Reasons include an invalid app-key, invalid app-secret or no valid deviceIds)\r\n");
            DEBUG_SINRIC("[SinricPro:handle()]:    -SinricPro is disabled! Check earlier log messages for details.\r\n");
            begin_error = true;
        }
        return;
    }

    if (!isConnected()) connect();
    _websocketListener.handle();
    _udpListener.handle();

    handleReceiveQueue();
    handleSendQueue();
}

JsonDocument SinricProClass::prepareRequest(String deviceId, const char* action) {
    JsonDocument requestMessage;
    JsonObject   header                     = requestMessage[FSTR_SINRICPRO_header].to<JsonObject>();
    header[FSTR_SINRICPRO_payloadVersion]   = 2;
    header[FSTR_SINRICPRO_signatureVersion] = 1;

    JsonObject payload                 = requestMessage[FSTR_SINRICPRO_payload].to<JsonObject>();
    payload[FSTR_SINRICPRO_action]     = action;
    payload[FSTR_SINRICPRO_createdAt]  = 0;
    payload[FSTR_SINRICPRO_deviceId]   = deviceId;
    payload[FSTR_SINRICPRO_replyToken] = MessageID().getID();
    payload[FSTR_SINRICPRO_type]       = FSTR_SINRICPRO_request;
    payload[FSTR_SINRICPRO_value].to<JsonObject>();
    return requestMessage;
}

void SinricProClass::handleResponse(JsonDocument& responseMessage) {
    (void)responseMessage;
    DEBUG_SINRIC("[SinricPro.handleResponse()]:\r\n");

#ifndef NODEBUG_SINRIC
    serializeJsonPretty(responseMessage, DEBUG_ESP_PORT);
    Serial.println();
#endif
}

void SinricProClass::handleModuleRequest(JsonDocument& requestMessage, interface_t Interface) {
    DEBUG_SINRIC("[SinricPro.handleModuleScopeRequest()]: handling device sope request\r\n");
#ifndef NODEBUG_SINRIC
    serializeJsonPretty(requestMessage, DEBUG_ESP_PORT);
#endif

    JsonDocument responseMessage = prepareResponse(requestMessage);

    String           action         = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action] | "";
    JsonObject       request_value  = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
    JsonObject       response_value = responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
    SinricProRequest request{action, "", request_value, response_value};

    bool success = _moduleCommandHandler.handleRequest(request);

    responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_success] = success;
    responseMessage[FSTR_SINRICPRO_payload].remove(FSTR_SINRICPRO_deviceId);
    if (!success) {
        if (responseMessageStr.length() > 0) {
            responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_message] = responseMessageStr;
            responseMessageStr                                              = "";
        } else {
            responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_message] = "Module did not handle \"" + action + "\"";
        }
    }

    String responseString;
    serializeJson(responseMessage, responseString);
    sendQueue.push(new SinricProMessage(Interface, responseString.c_str()));
}

void SinricProClass::handleDeviceRequest(JsonDocument& requestMessage, interface_t Interface) {
    DEBUG_SINRIC("[SinricPro.handleDeviceRequest()]: handling device sope request\r\n");
#ifndef NODEBUG_SINRIC
    serializeJsonPretty(requestMessage, DEBUG_ESP_PORT);
#endif

    JsonDocument responseMessage = prepareResponse(requestMessage);

    // handle devices
    bool        success        = false;
    const char* deviceId       = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_deviceId];
    String      action         = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action] | "";
    String      instance       = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId] | "";
    JsonObject  request_value  = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
    JsonObject  response_value = responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];

    for (auto& device : devices) {
        if (device->getDeviceId() == deviceId && success == false) {
            SinricProRequest request{
                action,
                instance,
                request_value,
                response_value};
            success                                                         = device->handleRequest(request);
            responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_success] = success;
            if (!success) {
                if (responseMessageStr.length() > 0) {
                    responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_message] = responseMessageStr;
                    responseMessageStr                                              = "";
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

    DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: %i message(s) in receiveQueue\r\n", receiveQueue.size());
    while (receiveQueue.size() > 0) {
        SinricProMessage* rawMessage = receiveQueue.front();
        receiveQueue.pop();
        JsonDocument jsonMessage;
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
            DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: Signature is valid. Processing message...\r\n");
            extractTimestamp(jsonMessage);
            if (messageType == FSTR_SINRICPRO_response) handleResponse(jsonMessage);
            if (messageType == FSTR_SINRICPRO_request) {
                String scope = jsonMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_scope] | FSTR_SINRICPRO_device;
                if (strcmp(FSTR_SINRICPRO_module, scope.c_str()) == 0) {
                    handleModuleRequest(jsonMessage, rawMessage->getInterface());
                } else {
                    handleDeviceRequest(jsonMessage, rawMessage->getInterface());
                }
            };
        } else {
            DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: Signature is invalid! \r\n");
            if (messageType == FSTR_SINRICPRO_request) handleDeviceRequest(jsonMessage, rawMessage->getInterface());
        }
        delete rawMessage;
    }
}

void SinricProClass::handleSendQueue() {
    if (!isConnected()) return;
    if (!timestamp.getTimestamp()) return;
    while (sendQueue.size() > 0) {
        DEBUG_SINRIC("[SinricPro:handleSendQueue()]: %i message(s) in sendQueue\r\n", sendQueue.size());
        DEBUG_SINRIC("[SinricPro:handleSendQueue()]: Sending message...\r\n");

        SinricProMessage* rawMessage = sendQueue.front();
        sendQueue.pop();

        JsonDocument jsonMessage;
        deserializeJson(jsonMessage, rawMessage->getMessage());
        jsonMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_createdAt] = timestamp.getTimestamp();
        signMessage(appSecret, jsonMessage);

        String messageStr;

        serializeJson(jsonMessage, messageStr);
#ifndef NODEBUG_SINRIC
        serializeJsonPretty(jsonMessage, DEBUG_ESP_PORT);
        Serial.println();
#endif

        switch (rawMessage->getInterface()) {
            case IF_WEBSOCKET:
                DEBUG_SINRIC("[SinricPro:handleSendQueue]: Sending to websocket\r\n");
                _websocketListener.sendMessage(messageStr);
                break;
            case IF_UDP:
                DEBUG_SINRIC("[SinricPro:handleSendQueue]: Sending to UDP\r\n");
                _udpListener.sendMessage(messageStr);
                break;
            default:
                break;
        }
        delete rawMessage;
        DEBUG_SINRIC("[SinricPro:handleSendQueue()]: message sent.\r\n");
    }
}

void SinricProClass::connect() {
    String deviceList;
    int    i = 0;
    for (auto& device : devices) {
        String deviceId = device->getDeviceId();
        if (i > 0) deviceList += ';';
        deviceList += device->getDeviceId();
        i++;
    }

    _websocketListener.begin(serverURL, appKey, deviceList, &receiveQueue);
}

void SinricProClass::stop() {
    _begin = false;
    DEBUG_SINRIC("[SinricPro:stop()\r\n");
    _websocketListener.stop();
}

bool SinricProClass::isConnected() {
    return _websocketListener.isConnected();
};

/**
 * @brief Set callback function for OTA (Over-The-Air) updates.
 *
 * This method registers a callback function that will be called when an OTA update is available.
 * The callback should handle the process of downloading and applying the update.
 *
 * @param cb A function pointer or lambda of type OTAUpdateCallbackHandler.
 *           The callback should return a boolean indicating whether the update was successful.
 *
 */
void SinricProClass::onOTAUpdate(OTAUpdateCallbackHandler cb) {
    _moduleCommandHandler.onOTAUpdate(cb);
}

/**
 * @brief Set callback function for setting a module setting.
 *
 * This method registers a callback function that will be called when a request to change
 * a module setting is received.
 * @return void
 * @param cb A function pointer or lambda of type SetSettingCallbackHandler.
 *           The callback should return a boolean indicating whether the setting was successfully updated.
 *
 */
void SinricProClass::onSetSetting(SetSettingCallbackHandler cb) {
    _moduleCommandHandler.onSetSetting(cb);
}

/**
 * @brief Sets the callback function for reporting device health status.
 *
 * This method allows the user to set a custom callback function that will be called
 * when the SinricPro system needs to report the device's health status.
 *
 * @param cb A function pointer of type ReportHealthCallbackHandler.
 *           This callback should populate a String with health information and return a boolean
 *           indicating success or failure of the health reporting process.
 * @return void
 * @see ReportHealthCallbackHandler for the definition of the callback function type.
 */
void SinricProClass::onReportHealth(ReportHealthCallbackHandler cb) {
    _moduleCommandHandler.onReportHealth(cb);
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
    _websocketListener.onConnected(cb);
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
    _websocketListener.onDisconnected(cb);
}

void SinricProClass::onPong(PongCallback cb) {
    _websocketListener.onPong(cb);
}

void SinricProClass::reconnect() {
    DEBUG_SINRIC("SinricPro:reconnect(): disconnecting\r\n");
    stop();
    DEBUG_SINRIC("SinricPro:reconnect(): connecting\r\n");
    connect();
}

void SinricProClass::onConnect() {
    DEBUG_SINRIC("[SinricPro]: Connected to \"%s\"!]\r\n", serverURL.c_str());
}

void SinricProClass::onDisconnect() {
    DEBUG_SINRIC("[SinricPro]: Disconnect\r\n");
}

void SinricProClass::extractTimestamp(JsonDocument& message) {
    unsigned long tempTimestamp = 0;
    // extract timestamp from timestamp message right after websocket connection is established
    tempTimestamp = message["timestamp"] | 0;
    if (tempTimestamp) {
        timestamp.setTimestamp(tempTimestamp);
        DEBUG_SINRIC("[SinricPro:extractTimestamp(): Got Timestamp %lu\r\n", tempTimestamp);
        return;
    }

    // extract timestamp from request message
    tempTimestamp = message[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_createdAt] | 0;
    if (tempTimestamp) {
        DEBUG_SINRIC("[SinricPro:extractTimestamp(): Got Timestamp %lu\r\n", tempTimestamp);
        timestamp.setTimestamp(tempTimestamp);
        return;
    }
}

void SinricProClass::sendMessage(JsonDocument& jsonMessage) {
    if (!isConnected()) {
        DEBUG_SINRIC("[SinricPro:sendMessage()]: device is offline, message has been dropped\r\n");
        return;
    }
    DEBUG_SINRIC("[SinricPro:sendMessage()]: pushing message into sendQueue\r\n");
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
    _websocketListener.setRestoreDeviceStates(flag);
}

/**
 * @brief operator[] is used tor create a new device instance or get an existing device instance
 *
 * If the device is unknown to SinricProClass it will create a new device instance
 * @param deviceId a String containing deviceId for device that have to been created or retreived
 * @return returns a proxy object representing the reference to a device derrivered from SinricProDevice
 * @section Syntax
 * `<DeviceType> &reference = SinricPro[<DEVICE_ID>];`
 * @section operator[] Example-Code
 * @code
 * #define SWITCH_ID         "YOUR-DEVICE-ID"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
 * ..
 *   SinricProSwitch &mySwitch = SinricPro[SWITCH_ID];
 * ..
 * @endcode
 **/
SinricProClass::Proxy SinricProClass::operator[](const String deviceId) {
    return Proxy(this, deviceId);
}

void SinricProClass::setResponseMessage(String&& message) {
    responseMessageStr = message;
}

/**
 * @brief
 *
 * return unsigned long current timestamp(unix epoch time) * /
 */
unsigned long SinricProClass::getTimestamp() {
    return timestamp.getTimestamp();
}

String SinricProClass::sign(const String& message) {
    return HMACbase64(message, appSecret);
}

JsonDocument SinricProClass::prepareResponse(JsonDocument& requestMessage) {
    JsonDocument responseMessage;
    JsonObject   header                     = responseMessage[FSTR_SINRICPRO_header].to<JsonObject>();
    header[FSTR_SINRICPRO_payloadVersion]   = 2;
    header[FSTR_SINRICPRO_signatureVersion] = 1;

    JsonObject payload                = responseMessage[FSTR_SINRICPRO_payload].to<JsonObject>();
    payload[FSTR_SINRICPRO_action]    = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action];
    payload[FSTR_SINRICPRO_clientId]  = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_clientId];
    payload[FSTR_SINRICPRO_scope]     = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_scope];
    payload[FSTR_SINRICPRO_createdAt] = 0;
    payload[FSTR_SINRICPRO_deviceId]  = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_deviceId];
    if (requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId].is<String>()) payload[FSTR_SINRICPRO_instanceId] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId];
    payload[FSTR_SINRICPRO_message]    = FSTR_SINRICPRO_OK;
    payload[FSTR_SINRICPRO_replyToken] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_replyToken];
    payload[FSTR_SINRICPRO_success]    = false;
    payload[FSTR_SINRICPRO_type]       = FSTR_SINRICPRO_response;
    payload[FSTR_SINRICPRO_value].to<JsonObject>();
    return responseMessage;
}

JsonDocument SinricProClass::prepareEvent(String deviceId, const char* action, const char* cause) {
    JsonDocument eventMessage;
    JsonObject   header                     = eventMessage[FSTR_SINRICPRO_header].to<JsonObject>();
    header[FSTR_SINRICPRO_payloadVersion]   = 2;
    header[FSTR_SINRICPRO_signatureVersion] = 1;

    JsonObject payload             = eventMessage[FSTR_SINRICPRO_payload].to<JsonObject>();
    payload[FSTR_SINRICPRO_action] = action;
    payload[FSTR_SINRICPRO_cause][FSTR_SINRICPRO_type].to<JsonObject>();
    payload[FSTR_SINRICPRO_cause][FSTR_SINRICPRO_type] = cause;
    payload[FSTR_SINRICPRO_createdAt]                  = 0;
    payload[FSTR_SINRICPRO_deviceId]                   = deviceId;
    payload[FSTR_SINRICPRO_replyToken]                 = MessageID().getID();
    payload[FSTR_SINRICPRO_type]                       = FSTR_SINRICPRO_event;
    payload[FSTR_SINRICPRO_value].to<JsonObject>();
    return eventMessage;
}

}  // namespace SINRICPRO_NAMESPACE

SINRICPRO_NAMESPACE::SinricProClass SinricPro;