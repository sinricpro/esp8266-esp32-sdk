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
#include "SinricProId.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using ConnectedCallbackHandler    = std::function<void(void)>;
using DisconnectedCallbackHandler = std::function<void(void)>;

class SinricProClass : public SinricProInterface {
  friend class SinricProDevice;
  public:
    void begin(AppKey socketAuthToken, AppSecret signingKey, String serverURL = SINRICPRO_SERVER_URL);

    void handle();
    void stop();
    bool isConnected();

    void onConnected(ConnectedCallbackHandler cb);
    void onDisconnected(DisconnectedCallbackHandler cb);
    void onPong(std::function<void(uint32_t)> cb) { _websocketListener.onPong(cb); }

    void restoreDeviceStates(bool flag);

    struct proxy {
      proxy(SinricProClass* ptr, DeviceId deviceId) : ptr(ptr), deviceId(deviceId) {}
      SinricProClass* ptr;
      DeviceId deviceId;
      template <typename DeviceType>
      operator DeviceType&() { return as<DeviceType>(); }
      template <typename DeviceType>
      DeviceType& as() { return ptr->getDeviceInstance<DeviceType>(deviceId); }
    };
    proxy operator[](const DeviceId deviceId) { return proxy(this, deviceId); }

    void setResponseMessage(String &&message) { responseMessageStr = message; }
    unsigned long getTimestamp() override { return baseTimestamp + (millis()/1000); }
  protected:
    template <typename DeviceType>
    DeviceType &add(DeviceId deviceId);

    void add(SinricProDeviceInterface *newDevice);

    void registerDevice(SinricProDeviceInterface *device);
    void unregisterDevice(SinricProDeviceInterface *device);

    DynamicJsonDocument prepareResponse(JsonDocument &requestMessage);
    DynamicJsonDocument prepareEvent(DeviceId deviceId, const char *action, const char *cause) override;
    void sendMessage(JsonDocument &jsonMessage) override;

  private:
    void handleReceiveQueue();
    void handleSendQueue();

    void handleRequest(DynamicJsonDocument& requestMessage, interface_t Interface);
    void handleResponse(DynamicJsonDocument& responseMessage);

    DynamicJsonDocument prepareRequest(DeviceId deviceId, const char* action);

    void connect();
    void disconnect();
    void reconnect();

    void onConnect() { DEBUG_SINRIC("[SinricPro]: Connected to \"%s\"!]\r\n", serverURL.c_str()); }
    void onDisconnect() { DEBUG_SINRIC("[SinricPro]: Disconnect\r\n"); }

    void extractTimestamp(JsonDocument &message);

    SinricProDeviceInterface* getDevice(DeviceId deviceId);

    template <typename DeviceType>
    DeviceType& getDeviceInstance(DeviceId deviceId);

    std::vector<SinricProDeviceInterface*> devices;

    AppKey socketAuthToken;
    AppSecret signingKey;
    String serverURL;

    websocketListener _websocketListener;
    udpListener _udpListener;
    SinricProQueue_t receiveQueue;
    SinricProQueue_t sendQueue;

    unsigned long baseTimestamp = 0;

    bool _begin = false;
    String responseMessageStr = "";
};

SinricProDeviceInterface* SinricProClass::getDevice(DeviceId deviceId) {
  for (auto& device : devices) {
    if (deviceId == device->getDeviceId()) return device;
  }
  return nullptr;
}

template <typename DeviceType>
DeviceType& SinricProClass::getDeviceInstance(DeviceId deviceId) { 
  DeviceType* tmp_device = (DeviceType*) getDevice(deviceId);
  if (tmp_device) return *tmp_device;
  
  DEBUG_SINRIC("[SinricPro]: Device \"%s\" does not exist. Creating new device\r\n", deviceId.toString().c_str());
  DeviceType& tmp_deviceInstance = add<DeviceType>(deviceId);

  return tmp_deviceInstance;
}

void SinricProClass::begin(AppKey socketAuthToken, AppSecret signingKey, String serverURL) {
  bool success = true;
  if (!socketAuthToken.isValid()) {
    DEBUG_SINRIC("[SinricPro:begin()]: App-Key \"%s\" is invalid!! Please check your app-key!! SinricPro will not work!\r\n", socketAuthToken.toString().c_str());
    success = false;
  }
  if (!signingKey.isValid()) {
    DEBUG_SINRIC("[SinricPro:begin()]: App-Secret \"%s\" is invalid!! Please check your app-secret!! SinricPro will not work!\r\n", signingKey.toString().c_str());
    success = false;
  }

  if(!success) {
    _begin = false;
    return;
  }

  this->socketAuthToken = socketAuthToken;
  this->signingKey = signingKey;
  this->serverURL = serverURL;
  _begin = true;
  _udpListener.begin(&receiveQueue);
}

template <typename DeviceType>
DeviceType& SinricProClass::add(DeviceId deviceId) {
  DeviceType* newDevice = new DeviceType(deviceId);
  if (DeviceId(deviceId).isValid()){
    DEBUG_SINRIC("[SinricPro:add()]: Adding device with id \"%s\".\r\n", deviceId.toString().c_str());
    if (socketAuthToken.isValid() && signingKey.isValid()) _begin = true;
  } else {
    DEBUG_SINRIC("[SinricPro:add()]: DeviceId \"%s\" is invalid!! Device will be ignored and will NOT WORK!\r\n", deviceId.toString().c_str());
  }
  return *newDevice;
}

void SinricProClass::registerDevice(SinricProDeviceInterface* device) {
  devices.push_back(device);
}

void SinricProClass::unregisterDevice(SinricProDeviceInterface *device) {
  devices.erase(std::remove(devices.begin(), devices.end(), device), devices.end());
}

void SinricProClass::handle() {
  static bool begin_error = false;
  if (!_begin) {
    if (!begin_error) { // print this only once!
      DEBUG_SINRIC("[SinricPro:handle()]: ERROR! SinricPro.begin() failed or was not called prior to event handler\r\n");
      DEBUG_SINRIC("[SinricPro:handle()]:    -Reasons include an invalid app-key, invalid app-secret or no valid deviceIds)\r\n");
      DEBUG_SINRIC("[SinricPro:handle()]:    -SinricPro is disabled! Check earlier log messages for details.\r\n");
      begin_error = true;
    }
    return;
  }

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

DynamicJsonDocument SinricProClass::prepareRequest(DeviceId deviceId, const char* action) {
  DynamicJsonDocument requestMessage(1024);
  JsonObject header = requestMessage.createNestedObject("header");
  header["payloadVersion"] = 2;
  header["signatureVersion"] = 1;

  JsonObject payload = requestMessage.createNestedObject("payload");
  payload["action"] = action;
  payload["createdAt"] = 0;
  payload["deviceId"] = deviceId.toString();
  payload["replyToken"] = MessageID().getID();
  payload["type"] = "request";
  payload.createNestedObject("value");
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
  const char* deviceId = requestMessage["payload"]["deviceId"];
  String action = requestMessage["payload"]["action"] | "";
  String instance = requestMessage["payload"]["instanceId"] | "";
  JsonObject request_value = requestMessage["payload"]["value"];
  JsonObject response_value = responseMessage["payload"]["value"];

  for (auto& device : devices) {
    if (device->getDeviceId() == deviceId && success == false) {
      SinricProRequest request {
        action,
        instance,
        request_value,
        response_value
      };
      success = device->handleRequest(request);
      responseMessage["payload"]["success"] = success;
      if (!success) {
        if (responseMessageStr.length() > 0){
          responseMessage["payload"]["message"] = responseMessageStr;
          responseMessageStr = ""; 
        } 
      } else {
        responseMessage["payload"]["message"] = "OK";
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
      String signature = jsonMessage["signature"]["HMAC"] | "";
      String payload = extractPayload(rawMessage->getMessage());
      String calculatedSignature = calculateSignature(signingKey.toString().c_str(), payload);
      sigMatch = (calculatedSignature == signature);
    }

    String messageType = jsonMessage["payload"]["type"];

    if (sigMatch) { // signature is valid process message
      DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: Signature is valid. Processing message...\r\n");
      extractTimestamp(jsonMessage);
      if (messageType == "response") handleResponse(jsonMessage);
      if (messageType == "request") handleRequest(jsonMessage, rawMessage->getInterface());
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
    jsonMessage["payload"]["createdAt"] = getTimestamp();
    signMessage(signingKey.toString(), jsonMessage);

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
  _websocketListener.begin(serverURL, socketAuthToken.toString(), &receiveQueue);
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
  tempTimestamp = message["timestamp"] | 0;
  if (tempTimestamp) {
    baseTimestamp = tempTimestamp - (millis() / 1000);
    DEBUG_SINRIC("[SinricPro:extractTimestamp(): Got Timestamp %lu\r\n", tempTimestamp);
    return;
  }

  tempTimestamp = message["payload"]["createdAt"] | 0;
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
  JsonObject header = responseMessage.createNestedObject("header");
  header["payloadVersion"] = 2;
  header["signatureVersion"] = 1;

  JsonObject payload = responseMessage.createNestedObject("payload");
  payload["action"] = requestMessage["payload"]["action"];
  payload["clientId"] = requestMessage["payload"]["clientId"];
  payload["createdAt"] = 0;
  payload["deviceId"] = requestMessage["payload"]["deviceId"];
  if (requestMessage["payload"].containsKey("instanceId")) payload["instanceId"] = requestMessage["payload"]["instanceId"];
  String action = requestMessage["payload"]["action"];
  payload["message"] = "Device did not handle \""+ action + "\"";
  payload["replyToken"] = requestMessage["payload"]["replyToken"];
  payload["success"] = false;
  payload["type"] = "response";
  payload.createNestedObject("value");
  return responseMessage;
}


DynamicJsonDocument SinricProClass::prepareEvent(DeviceId deviceId, const char* action, const char* cause) {
  DynamicJsonDocument eventMessage(1024);
  JsonObject header = eventMessage.createNestedObject("header");
  header["payloadVersion"] = 2;
  header["signatureVersion"] = 1;

  JsonObject payload = eventMessage.createNestedObject("payload");
  payload["action"] = action;
  payload["cause"].createNestedObject("type");
  payload["cause"]["type"] = cause;
  payload["createdAt"] = 0;
  payload["deviceId"] = deviceId.toString();
  payload["replyToken"] = MessageID().getID();
  payload["type"] = "event";
  payload.createNestedObject("value");
  return eventMessage;
}

} // SINRICPRO_NAMESPACE

using namespace SINRICPRO_NAMESPACE;

SinricProClass SinricPro;