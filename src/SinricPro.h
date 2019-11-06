/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SINRIC_H_
#define _SINRIC_H_

#include "SinricProInterface.h"
#include "SinricProDeviceInterface.h"
#include "SinricProWebsocket.h"
#include "SinricProUDP.h"
#include "SinricProSignature.h"
#include "SinricProMessageid.h"
#include "SinricProQueue.h"

class SinricProClass : public SinricProInterface {
  public:
    void begin(String socketAuthToken, String signingKey, String serverURL = SINRICPRO_SERVER_URL);
    template <typename DeviceType>
    DeviceType& add(const char* deviceId, unsigned long eventWaitTime = 1000);

    void add(SinricProDeviceInterface& newDevice);
    void add(SinricProDeviceInterface* newDevice);
    void handle();
    void stop();
    bool isConnected();

    typedef std::function<void(void)> connectCallbackHandler;
    void onConnected(connectCallbackHandler cb) { _websocketListener.onConnected(cb); }
    void onDisconnected(connectCallbackHandler cb) { _websocketListener.onDisconnected(cb); }

    void restoreDeviceStates(bool flag) { _websocketListener.setRestoreDeviceStates(flag); }

    DynamicJsonDocument prepareResponse(JsonDocument& requestMessage);
    DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause) override;
    void sendMessage(JsonDocument& jsonMessage) override;
  
    struct proxy {
      proxy(SinricProClass* ptr, String deviceId) : ptr(ptr), deviceId(deviceId) {}
      SinricProClass* ptr;
      String deviceId;
      template <typename DeviceType>
      operator DeviceType&() { return as<DeviceType>(); }
      template <typename DeviceType>
      DeviceType& as() { return ptr->getDeviceInstance<DeviceType>(deviceId); }
    };
    
    proxy operator[](const String deviceId) { return proxy(this, deviceId); }

  private:
    void handleReceiveQueue();
    void handleSendQueue();
    
    void handleRequest(DynamicJsonDocument& requestMessage, interface_t Interface);
    void handleResponse(DynamicJsonDocument& responseMessage);

    DynamicJsonDocument prepareRequest(const char* deviceId, const char* action);

    void connect();
    void disconnect();
    void reconnect();

    void onConnect() { DEBUG_SINRIC("[SinricPro:onConnect()]\r\n"); }
    void onDisconnect() { DEBUG_SINRIC("[SinricPro:onDisconnect()]\r\n"); }

    bool checkDeviceId(String deviceId);
    void extractTimestamp(JsonDocument &message);

    SinricProDeviceInterface* getDevice(String deviceId);
    
    template <typename DeviceType>
    DeviceType& getDeviceInstance(String deviceId) { return (DeviceType&) *getDevice(deviceId); }

    std::vector<SinricProDeviceInterface*> devices;
    String socketAuthToken;
    String signingKey;
    String serverURL;

    websocketListener _websocketListener;
    udpListener _udpListener;
    SinricProQueue_t receiveQueue;
    SinricProQueue_t sendQueue;

    unsigned long getTimestamp() { return baseTimestamp + (millis()/1000); }
    unsigned long baseTimestamp = 0;
};

SinricProDeviceInterface* SinricProClass::getDevice(String deviceId) {
  for (auto& device : devices) {
    if (deviceId == String(device->getDeviceId())) return device;
  }
  return nullptr;
}

void SinricProClass::begin(String socketAuthToken, String signingKey, String serverURL) {
  this->socketAuthToken = socketAuthToken;
  this->signingKey = signingKey;
  this->serverURL = serverURL;
}

template <typename DeviceType>
DeviceType& SinricProClass::add(const char* deviceId, unsigned long eventWaitTime) {
  DeviceType* newDevice = new DeviceType(deviceId, eventWaitTime);
  if (checkDeviceId(String(deviceId))){
    newDevice->begin(this);
    devices.push_back(newDevice);
  }
  return *newDevice;
}

__attribute__ ((deprecated("Please use DeviceType& myDevice = SinricPro.add<DeviceType>(DeviceId);")))
void SinricProClass::add(SinricProDeviceInterface* newDevice) {
  if (!checkDeviceId(String(newDevice->getDeviceId()))) return;
  newDevice->begin(this);
  devices.push_back(newDevice);
}

__attribute__ ((deprecated("Please use DeviceType& myDevice = SinricPro.add<DeviceType>(DeviceId);")))
void SinricProClass::add(SinricProDeviceInterface& newDevice) {
  if (!checkDeviceId(String(newDevice.getDeviceId()))) return;
  newDevice.begin(this);
  devices.push_back(&newDevice);
}

void SinricProClass::handle() {
  if (!isConnected()) connect();
  _websocketListener.handle();
  _udpListener.handle();

  handleReceiveQueue();
  handleSendQueue();
}

DynamicJsonDocument SinricProClass::prepareRequest(const char* deviceId, const char* action) {
  DynamicJsonDocument requestMessage(1024);
  JsonObject header = requestMessage.createNestedObject("header");
  header["payloadVersion"] = 2;
  header["signatureVersion"] = 1;

  JsonObject payload = requestMessage.createNestedObject("payload");
  payload["action"] = action;
  payload["createdAt"] = 0;
  payload["deviceId"] = deviceId;
  payload["replyToken"] = MessageID().getID();
  payload["type"] = "request";
  payload.createNestedObject("value");
  return requestMessage;
}

void SinricProClass::handleResponse(DynamicJsonDocument& responseMessage) {
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
  const char* action = requestMessage["payload"]["action"];
  JsonObject request_value = requestMessage["payload"]["value"];
  JsonObject response_value = responseMessage["payload"]["value"];

  for (auto& device : devices) {
    if (strcmp(deviceId, device->getDeviceId()) == 0 && success == false) {
      success = device->handleRequest(deviceId, action, request_value, response_value);
      responseMessage["payload"]["success"] = success;
    }
  }
  
  String responseString;
  serializeJson(responseMessage, responseString);
  sendQueue.push(new SinricProMessage(Interface, responseString.c_str()));
}

void SinricProClass::handleReceiveQueue() {
  if (receiveQueue.count() == 0) return;

  DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: %i message(s) in receiveQueue\r\n", receiveQueue.count());
  while (receiveQueue.count() > 0) {  
    SinricProMessage* rawMessage = receiveQueue.pop();
    DynamicJsonDocument jsonMessage(1024);
    deserializeJson(jsonMessage, rawMessage->getMessage());
    
    bool sigMatch = false;

    if (strncmp(rawMessage->getMessage(), "{\"timestamp\":", 13) == 0 && strlen(rawMessage->getMessage()) <= 26) {
      sigMatch=true; // timestamp message has no signature...ignore sigMatch for this!
    } else {
      sigMatch = verifyMessage(signingKey, jsonMessage);
    }
    
    String messageType = jsonMessage["payload"]["type"];

    if (sigMatch) { // signature is valid process message
      DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: Signature is valid! Processing message.\r\n");
      extractTimestamp(jsonMessage);
      if (messageType == "response") handleResponse(jsonMessage);
      if (messageType == "request") handleRequest(jsonMessage, rawMessage->getInterface());
    } else {
      DEBUG_SINRIC("[SinricPro.handleReceiveQueue()]: Signature is invalid...sending messsage to [dev/null] ;)\r\n");
    }
    delete rawMessage;
  }
}

void SinricProClass::handleSendQueue() {
  if (!isConnected()) return;
  if (!baseTimestamp) return;
  while (sendQueue.count() > 0) {
    DEBUG_SINRIC("[SinricPro:handleSendQueue()]: %i message(s) in sendQueue\r\n", sendQueue.count());
    DEBUG_SINRIC("[SinricPro:handleSendQueue()]: Sending message...\r\n");

    SinricProMessage* rawMessage = sendQueue.pop();

    DynamicJsonDocument jsonMessage(1024);
    deserializeJson(jsonMessage, rawMessage->getMessage());
    jsonMessage["payload"]["createdAt"] = getTimestamp();
    signMessage(signingKey, jsonMessage);
    
    String messageStr;

    serializeJson(jsonMessage, messageStr);
    #ifndef NODEBUG_SINRIC
            serializeJsonPretty(jsonMessage, DEBUG_ESP_PORT);
            Serial.println();
    #endif

    switch (rawMessage->getInterface()) {
      case IF_WEBSOCKET: _websocketListener.sendMessage(messageStr); break;
      case IF_UDP:       _udpListener.sendMessage(messageStr); break;
      default:           break;
    }
    delete rawMessage;
    DEBUG_SINRIC("[SinricPro:handleSendQueue()]: message sent.\r\n");
  }
}

void SinricProClass::connect() {
  String deviceList;
  int i = 0;
  for (auto& device : devices) {
    if (i>0) deviceList += ";";
    deviceList += String(device->getDeviceId());
    i++;
  }

  _websocketListener.begin(serverURL, socketAuthToken, deviceList.c_str(), &receiveQueue);
}


void SinricProClass::stop() {
  DEBUG_SINRIC("[SinricPro:stop()\r\n");
  _websocketListener.stop();
}

bool SinricProClass::isConnected() {
  return _websocketListener.isConnected();
};


void SinricProClass::reconnect() {
  DEBUG_SINRIC("SinricProClass.reconnect(): disconnecting\r\n");
  stop();
  DEBUG_SINRIC("SinricProClass.reconnect(): wait 1second\r\n");
  delay(1000);
  DEBUG_SINRIC("SinricProClass.reconnect(): connecting\r\n");
  connect();
}

bool SinricProClass::checkDeviceId(String deviceId) {
  if (deviceId.length() != 24) {
    DEBUG_SINRIC("[SinricPro.add()]: Invalid deviceId \"%s\"! Device will be ignored!\r\n", deviceId.c_str());
    return false;
  }

  for (size_t i = 0; i < deviceId.length(); i++) {
    char current = deviceId[i];
    if (current >= '0' && current <= '9') continue;
    if (current >= 'A' && current <= 'F') continue;
    if (current >= 'a' && current <= 'f') continue;
    DEBUG_SINRIC("[SinricPro.add()]: Invalid deviceId \"%s\"! Device will be ignored!\r\n", deviceId.c_str());
    return false;
  }
  return true;
}

void SinricProClass::extractTimestamp(JsonDocument &message) {
  unsigned long tempTimestamp = 0;
  // extract timestamp from timestamp message right after websocket connection is established
  tempTimestamp = message["timestamp"] | 0;
  if (tempTimestamp) {
    baseTimestamp = tempTimestamp - (millis() / 1000);
    DEBUG_SINRIC("[SinricPro:extractTimestamp(): Got Timestamp %lu\r\n", tempTimestamp);
    return;
  }

  // extract timestamp from request message
  tempTimestamp = message["payload"]["createdAt"] | 0;
  if (tempTimestamp) {
    baseTimestamp = tempTimestamp - (millis() / 1000);
    return;
  }
}


void SinricProClass::sendMessage(JsonDocument& jsonMessage) {
  DEBUG_SINRIC("[SinricPro:sendMessage()]: pushing message into sendQueue\r\n");
  String messageString;
  serializeJson(jsonMessage, messageString);
  sendQueue.push(new SinricProMessage(IF_WEBSOCKET, messageString.c_str()));
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
  payload["message"] = "OK";
  payload["replyToken"] = requestMessage["payload"]["replyToken"];
  payload["success"] = false;
  payload["type"] = "response";
  payload.createNestedObject("value");
  return responseMessage;
}


DynamicJsonDocument SinricProClass::prepareEvent(const char* deviceId, const char* action, const char* cause) {
  DynamicJsonDocument eventMessage(1024);
  JsonObject header = eventMessage.createNestedObject("header");
  header["payloadVersion"] = 2;
  header["signatureVersion"] = 1;

  JsonObject payload = eventMessage.createNestedObject("payload");
  payload["action"] = action;
  payload["cause"] = cause;
  payload["createdAt"] = 0;
  payload["deviceId"] = deviceId;
  payload["replyToken"] = MessageID().getID();
  payload["type"] = "event";
  payload.createNestedObject("value");
  return eventMessage;
}

#ifndef NOSINRIC_INSTANCE 
SinricProClass SinricPro;
#endif

#endif