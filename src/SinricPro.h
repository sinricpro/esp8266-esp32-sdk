/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include <functional>
#include "SinricProInterface.h"
#include "SinricProDeviceInterface.h"
#include "SinricProWebsocket.h"
#include "SinricProUDP.h"
#include "SinricProSignature.h"
#include "SinricProMessageid.h"
#include "SinricProQueue.h"
#include "SinricProStrings.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using ConnectedCallbackHandler    = std::function<void(void)>;
using DisconnectedCallbackHandler = std::function<void(void)>;

class SinricProClass : public SinricProInterface {
  friend class SinricProDevice;
  public:
    void begin(const String &appKey, const String &appSecret, const String &serverURL = SINRICPRO_SERVER_URL);

    void handle();
    void stop();
    bool isConnected();

    void onConnected(ConnectedCallbackHandler cb);
    void onDisconnected(DisconnectedCallbackHandler cb);
    void onPong(std::function<void(uint32_t)> cb) { _websocketListener.onPong(cb); }

    void restoreDeviceStates(bool flag);

    struct proxy {
      proxy(SinricProClass* ptr, const String &deviceId) : ptr(ptr), deviceId(deviceId) {}
      SinricProClass* ptr;
      String deviceId;
      template <typename DeviceType>
      operator DeviceType&() { return as<DeviceType>(); }
      template <typename DeviceType>
      DeviceType& as() { return ptr->getDeviceInstance<DeviceType>(deviceId); }
    };
    proxy operator[](const String &deviceId) { return proxy(this, deviceId); }

    void setResponseMessage(String &&message) { responseMessageStr = message; }
    unsigned long getTimestamp() override { return baseTimestamp + (millis()/1000); }
  protected:
    template <typename DeviceType>
    DeviceType &add(const String &deviceId);

    void add(SinricProDeviceInterface *newDevice);

    void registerDevice(SinricProDeviceInterface *device);
    void unregisterDevice(SinricProDeviceInterface *device);

    DynamicJsonDocument prepareResponse(JsonDocument &requestMessage);
    DynamicJsonDocument prepareEvent(const String &deviceId, const char *action, const char *cause) override;
    void sendMessage(JsonDocument &jsonMessage) override;

  private:
    void handleReceiveQueue();
    void handleSendQueue();

    void handleRequest(DynamicJsonDocument& requestMessage, interface_t Interface);
    void handleResponse(DynamicJsonDocument& responseMessage);

    DynamicJsonDocument prepareRequest(const String &deviceId, const char* action);

    void connect();
    void disconnect();
    void reconnect();

    void onConnect() { DEBUG_SINRIC("[SinricPro]: Connected to \"%s\"!]\r\n", serverURL.c_str()); }
    void onDisconnect() { DEBUG_SINRIC("[SinricPro]: Disconnect\r\n"); }

    void extractTimestamp(JsonDocument &message);

    SinricProDeviceInterface* getDevice(const String &deviceId);

    template <typename DeviceType>
    DeviceType& getDeviceInstance(const String &deviceId);

    std::vector<SinricProDeviceInterface*> devices;

    String appKey;
    String appSecret;
    String serverURL;

    WebsocketListener _websocketListener;
    udpListener _udpListener;
    SinricProQueue_t receiveQueue;
    SinricProQueue_t sendQueue;

    unsigned long baseTimestamp = 0;

    bool _begin = false;
    String responseMessageStr = "";
};

SinricProDeviceInterface* SinricProClass::getDevice(const String &deviceId) {
  for (auto& device : devices) {
    if (deviceId == device->getDeviceId()) return device;
  }
  return nullptr;
}

template <typename DeviceType>
DeviceType& SinricProClass::getDeviceInstance(const String &deviceId) { 
  DeviceType* tmp_device = (DeviceType*) getDevice(deviceId);
  if (tmp_device) return *tmp_device;
  
  DEBUG_SINRIC("[SinricPro]: Device \"%s\" does not exist. Creating new device\r\n", deviceId.c_str());
  DeviceType& tmp_deviceInstance = add<DeviceType>(deviceId);

  return tmp_deviceInstance;
}

void SinricProClass::begin(const String &appKey, const String &appSecret, const String &serverURL) {
  this->appKey    = appKey;
  this->appSecret = appSecret;
  this->serverURL = serverURL;
  _begin = true;
  _udpListener.begin(&receiveQueue);
}

template <typename DeviceType>
DeviceType& SinricProClass::add(const String &deviceId) {
  DeviceType* newDevice = new DeviceType(deviceId);
  return *newDevice;
}

void SinricProClass::registerDevice(SinricProDeviceInterface* device) {
  devices.push_back(device);
}

void SinricProClass::unregisterDevice(SinricProDeviceInterface *device) {
  devices.erase(std::remove(devices.begin(), devices.end(), device), devices.end());
}

void SinricProClass::handle() {
  if (devices.size()) {
    for (auto& device : devices) {
      device->loop();
      yield();
    }
  }

  if (!isConnected() && WiFi.isConnected()) connect();
  _websocketListener.handle();
  _udpListener.handle();

  handleReceiveQueue();
  handleSendQueue();
}

DynamicJsonDocument SinricProClass::prepareRequest(const String &deviceId, const char* action) {
  DynamicJsonDocument requestMessage(1024);
  JsonObject header = requestMessage.createNestedObject(FSTR_SINRICPRO_header);
  header[FSTR_SINRICPRO_payloadVersion] = 2;
  header[FSTR_SINRICPRO_signatureVersion] = 1;

  JsonObject payload = requestMessage.createNestedObject(FSTR_SINRICPRO_payload);
  payload[FSTR_SINRICPRO_action] = action;
  payload[FSTR_SINRICPRO_createdAt] = 0;
  payload[FSTR_SINRICPRO_deviceId] = deviceId;
  payload[FSTR_SINRICPRO_replyToken] = MessageID().getID();
  payload[FSTR_SINRICPRO_type] = FSTR_SINRICPRO_request;
  payload.createNestedObject(FSTR_SINRICPRO_value);
  return requestMessage;
}

void SinricProClass::handleResponse(DynamicJsonDocument& responseMessage) {
  (void) responseMessage;
  DEBUG_SINRIC("[SinricPro.handleResponse()]:\r\n");

  #ifndef NODEBUG_SINRIC
          serializeJsonPretty(responseMessage, DEBUG_ESP_PORT);
          Serial.println();
  #endif
}

void SinricProClass::handleRequest(DynamicJsonDocument& requestMessage, interface_t Interface) {
  DEBUG_SINRIC("[SinricPro.handleRequest()]: handling request\r\n");
  #ifndef NODEBUG_SINRIC
          serializeJsonPretty(requestMessage, DEBUG_ESP_PORT);
  #endif

  DynamicJsonDocument responseMessage = prepareResponse(requestMessage);

  // handle devices
  bool success = false;
  const char* deviceId = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_deviceId];
  String action = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action] | "";
  String instance = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId] | "";
  JsonObject request_value = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];
  JsonObject response_value = responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_value];

  for (auto& device : devices) {
    if (device->getDeviceId() == deviceId && success == false) {
      SinricProRequest request {
        action,
        instance,
        request_value,
        response_value
      };
      success = device->handleRequest(request);
      responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_success] = success;
      if (!success) {
        if (responseMessageStr.length() > 0){
          responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_message] = responseMessageStr;
          responseMessageStr = ""; 
        } 
      } else {
        responseMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_message] = "OK";
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
    DynamicJsonDocument jsonMessage(1024);
    deserializeJson(jsonMessage, rawMessage->getMessage());

    bool sigMatch = false;

    if (strncmp(rawMessage->getMessage(), "{\"timestamp\":", 13) == 0 && strlen(rawMessage->getMessage()) <= 26) {
      sigMatch=true; // timestamp message has no signature...ignore sigMatch for this!
    } else {
      String signature = jsonMessage[FSTR_SINRICPRO_signature][FSTR_SINRICPRO_HMAC] | "";
      String payload = extractPayload(rawMessage->getMessage());
      String calculatedSignature = calculateSignature(appSecret.c_str(), payload);
      sigMatch = (calculatedSignature == signature);
    }

    String messageType = jsonMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_type];

    if (sigMatch) { // signature is valid process message
      DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: Signature is valid. Processing message...\r\n");
      extractTimestamp(jsonMessage);
      if (messageType == FSTR_SINRICPRO_response) handleResponse(jsonMessage);
      if (messageType == FSTR_SINRICPRO_request) handleRequest(jsonMessage, rawMessage->getInterface());
    } else {
      DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: Signature is invalid! Sending messsage to [dev/null] ;)\r\n");
    }
    delete rawMessage;
  }
}

void SinricProClass::handleSendQueue() {
  if (!isConnected()) return;
  if (!baseTimestamp) return;
  while (sendQueue.size() > 0) {
    DEBUG_SINRIC("[SinricPro:handleSendQueue()]: %i message(s) in sendQueue\r\n", sendQueue.size());
    DEBUG_SINRIC("[SinricPro:handleSendQueue()]: Sending message...\r\n");

    SinricProMessage* rawMessage = sendQueue.front(); sendQueue.pop();

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
      case IF_WEBSOCKET: DEBUG_SINRIC("[SinricPro:handleSendQueue]: Sending to websocket\r\n"); _websocketListener.sendMessage(messageStr); break;
      case IF_UDP:       DEBUG_SINRIC("[SinricPro:handleSendQueue]: Sending to UDP\r\n");_udpListener.sendMessage(messageStr); break;
      default:           break;
    }
    delete rawMessage;
    DEBUG_SINRIC("[SinricPro:handleSendQueue()]: message sent.\r\n");
  }
}

void SinricProClass::connect() {
  _websocketListener.begin(serverURL, appKey, &receiveQueue);
}

void SinricProClass::stop() {
  DEBUG_SINRIC("[SinricPro:stop()\r\n");
  _websocketListener.stop();
}

bool SinricProClass::isConnected() {
  return _websocketListener.isConnected();
};

void SinricProClass::onConnected(ConnectedCallbackHandler cb) {
  _websocketListener.onConnected(cb);
}

void SinricProClass::onDisconnected(DisconnectedCallbackHandler cb) {
  _websocketListener.onDisconnected(cb);
}

void SinricProClass::reconnect() {
  DEBUG_SINRIC("SinricPro:reconnect(): disconnecting\r\n");
  stop();
  DEBUG_SINRIC("SinricPro:reconnect(): connecting\r\n");
  connect();
}

void SinricProClass::extractTimestamp(JsonDocument &message) {
  unsigned long tempTimestamp = 0;
  tempTimestamp = message[FSTR_SINRICPRO_timestamp] | 0;
  if (tempTimestamp) {
    baseTimestamp = tempTimestamp - (millis() / 1000);
    DEBUG_SINRIC("[SinricPro:extractTimestamp(): Got Timestamp %lu\r\n", tempTimestamp);
    return;
  }

  tempTimestamp = message[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_createdAt] | 0;
  if (tempTimestamp) {
    DEBUG_SINRIC("[SinricPro:extractTimestamp(): Got Timestamp %lu\r\n", tempTimestamp);
    baseTimestamp = tempTimestamp - (millis() / 1000);
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

void SinricProClass::restoreDeviceStates(bool flag) { 
  _websocketListener.setRestoreDeviceStates(flag);
}

DynamicJsonDocument SinricProClass::prepareResponse(JsonDocument& requestMessage) {
  DynamicJsonDocument responseMessage(1024);
  JsonObject header = responseMessage.createNestedObject(FSTR_SINRICPRO_header);
  header[FSTR_SINRICPRO_payloadVersion] = 2;
  header[FSTR_SINRICPRO_signatureVersion] = 1;

  JsonObject payload = responseMessage.createNestedObject(FSTR_SINRICPRO_payload);
  payload[FSTR_SINRICPRO_action] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action];
  payload[FSTR_SINRICPRO_clientId] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_clientId];
  payload[FSTR_SINRICPRO_createdAt] = 0;
  payload[FSTR_SINRICPRO_deviceId] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_deviceId];
  if (requestMessage[FSTR_SINRICPRO_payload].containsKey(FSTR_SINRICPRO_instanceId)) payload[FSTR_SINRICPRO_instanceId] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_instanceId];
  String action = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_action];
  payload[FSTR_SINRICPRO_message] = "Device did not handle \""+ action + "\"";
  payload[FSTR_SINRICPRO_replyToken] = requestMessage[FSTR_SINRICPRO_payload][FSTR_SINRICPRO_replyToken];
  payload[FSTR_SINRICPRO_success] = false;
  payload[FSTR_SINRICPRO_type] = FSTR_SINRICPRO_response;
  payload.createNestedObject(FSTR_SINRICPRO_value);
  return responseMessage;
}


DynamicJsonDocument SinricProClass::prepareEvent(const String &deviceId, const char* action, const char* cause) {
  DynamicJsonDocument eventMessage(1024);
  JsonObject header = eventMessage.createNestedObject(FSTR_SINRICPRO_header);
  header[FSTR_SINRICPRO_payloadVersion] = 2;
  header[FSTR_SINRICPRO_signatureVersion] = 1;

  JsonObject payload = eventMessage.createNestedObject(FSTR_SINRICPRO_payload);
  payload[FSTR_SINRICPRO_action] = action;
  payload[FSTR_SINRICPRO_cause].createNestedObject(FSTR_SINRICPRO_type);
  payload[FSTR_SINRICPRO_cause][FSTR_SINRICPRO_type] = cause;
  payload[FSTR_SINRICPRO_createdAt] = 0;
  payload[FSTR_SINRICPRO_deviceId] = deviceId;
  payload[FSTR_SINRICPRO_replyToken] = MessageID().getID();
  payload[FSTR_SINRICPRO_type] = FSTR_SINRICPRO_event;
  payload.createNestedObject(FSTR_SINRICPRO_value);
  return eventMessage;
}

} // SINRICPRO_NAMESPACE

using namespace SINRICPRO_NAMESPACE;

SinricProClass SinricPro;