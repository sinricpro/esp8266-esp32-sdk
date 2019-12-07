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

/**
 * @class SinricProClass
 * @brief The main class of this library, handling communication between SinricPro Server and your devices
 **/
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

    typedef std::function<void(void)> ConnectCallbackHandler;
    void onConnected(ConnectCallbackHandler cb);
    void onDisconnected(ConnectCallbackHandler cb);

    void restoreDeviceStates(bool flag);

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
    /**
     * @brief operator[] is used tor create a new device instance or get an existing device instance
     * 
     * If the device is unknown to SinricProClass it will create a new device instance
     * @param deviceId a String containing deviceId for device that have to been created or retreived
     * @return returns a proxy object representing the reference to a device derrivered from SinricProDevice
     * @subsubsection Syntax
     * `<DeviceType> &reference = SinricPro[<DEVICE_ID>];`
     * @subsubsection Example-Code
     * @code
     * #define SWITCH_ID         "YOUR-DEVICE-ID"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
     * ..
     *   SinricProSwitch &mySwitch = SinricPro[SWITCH_ID];
     * ..
     * @endcode
     **/ 
    proxy operator[](const String deviceId) { return proxy(this, deviceId); }

    // setResponseMessage is is just a workaround until verison 3.x.x will be released
    void setResponseMessage(String &&message) { responseMessageStr = message; }

  private:
    void handleReceiveQueue();
    void handleSendQueue();

    void handleRequest(DynamicJsonDocument& requestMessage, interface_t Interface);
    void handleResponse(DynamicJsonDocument& responseMessage);

    DynamicJsonDocument prepareRequest(const char* deviceId, const char* action);

    void connect();
    void disconnect();
    void reconnect();

    void onConnect() { DEBUG_SINRIC("[SinricPro]: Connected to \"%s\"!]\r\n", serverURL.c_str()); }
    void onDisconnect() { DEBUG_SINRIC("[SinricPro]: Disconnect\r\n"); }

    bool verifyDeviceId(const char* id);
    bool verifyAppKey(const char* key);
    bool verifyAppSecret(const char* secret);
    void extractTimestamp(JsonDocument &message);

    SinricProDeviceInterface* getDevice(String deviceId);

    template <typename DeviceType>
    DeviceType& getDeviceInstance(String deviceId);

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

    bool _begin = false;
    String responseMessageStr = "";
};

SinricProDeviceInterface* SinricProClass::getDevice(String deviceId) {
  for (auto& device : devices) {
    if (deviceId == String(device->getDeviceId())) return device;
  }
  return nullptr;
}

template <typename DeviceType>
DeviceType& SinricProClass::getDeviceInstance(String deviceId) { 
  DeviceType* tmp_device = (DeviceType*) getDevice(deviceId);
  if (tmp_device) return *tmp_device;
  
  DEBUG_SINRIC("[SinricPro]: Device \"%s\" does not exist. Creating new device\r\n", deviceId.c_str());
  DeviceType& tmp_deviceInstance = add<DeviceType>(deviceId.c_str());

  if (isConnected()) {
    DEBUG_SINRIC("[SinricPro]: Reconnecting to server.\r\n");
    reconnect();
  }

  return tmp_deviceInstance;
}

/**
 * @brief Initializing SinricProClass to be able to connect to SinricPro Server
 * 
 * @param socketAuthToken `String` containing APP_KEY (see credentials from https://sinric.pro )
 * @param signingKey `String` containing APP_SECRET (see credentials from https:://sinric.pro)
 * @param serverURL `String` containing SinricPro Server URL (default="ws.sinric.pro")
 * @subsubsection Example-Code
 * @code
 * #define APP_KEY           "YOUR-APP-KEY"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
 * #define APP_SECRET        "YOUR-APP-SECRET"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
 * 
 * void setup() {
 *   SinricPro.begin(APP_KEY, APP_SECRET);
 * }
 * @endcode
 **/
void SinricProClass::begin(String socketAuthToken, String signingKey, String serverURL) {
  bool success = true;
  if (!verifyAppKey(socketAuthToken.c_str())) {
    DEBUG_SINRIC("[SinricPro:begin()]: App-Key \"%s\" is invalid!! Please check your app-key!! SinricPro will not work!\r\n", socketAuthToken.c_str());
    success = false;
  }
  if (!verifyAppSecret(signingKey.c_str())) {
    DEBUG_SINRIC("[SinricPro:begin()]: App-Secret \"%s\" is invalid!! Please check your app-secret!! SinricPro will not work!\r\n", signingKey.c_str());
    success = false;
    return;
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
DeviceType& SinricProClass::add(const char* deviceId, unsigned long eventWaitTime) {
  DeviceType* newDevice = new DeviceType(deviceId, eventWaitTime);
  if (verifyDeviceId(deviceId)){
    DEBUG_SINRIC("[SinricPro:add()]: Adding device with id \"%s\".\r\n", deviceId);
    newDevice->begin(this);
    if (verifyAppKey(socketAuthToken.c_str()) && verifyAppSecret(signingKey.c_str())) _begin = true;
  } else {
    DEBUG_SINRIC("[SinricPro:add()]: DeviceId \"%s\" is invalid!! Device will be ignored and will NOT WORK!\r\n", deviceId);
  }
  devices.push_back(newDevice);
  return *newDevice;
}

__attribute__ ((deprecated("Please use DeviceType& myDevice = SinricPro.add<DeviceType>(DeviceId);")))
void SinricProClass::add(SinricProDeviceInterface* newDevice) {
  if (!verifyDeviceId(newDevice->getDeviceId())) return;
  newDevice->begin(this);
  devices.push_back(newDevice);
}

__attribute__ ((deprecated("Please use DeviceType& myDevice = SinricPro.add<DeviceType>(DeviceId);")))
void SinricProClass::add(SinricProDeviceInterface& newDevice) {
  if (!verifyDeviceId(newDevice.getDeviceId())) return;
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
 * @subsubsection Example-Code
 * @code
 * void loop() {
 *   SinricPro.handle();
 * }
 * @endcode
 **/
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
      if (!success) {
        if (responseMessageStr.length() > 0){
          responseMessage["payload"]["message"] = responseMessageStr;
          responseMessageStr = ""; 
        } else {
          responseMessage["payload"]["message"] = "Device returned an error while processing the request!";
        }
      }
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
      case IF_WEBSOCKET: DEBUG_SINRIC("[SinricPro:handleSendQueue]: Sending to websocket\r\n"); _websocketListener.sendMessage(messageStr); break;
      case IF_UDP:       DEBUG_SINRIC("[SinricPro:handleSendQueue]: Sending to UDP\r\n");_udpListener.sendMessage(messageStr); break;
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
    const char* deviceId = device->getDeviceId();
    if (verifyDeviceId(deviceId)) {
      if (i>0) deviceList += ';';
      deviceList += String(deviceId);
      i++;
    }
  }
  if (i==0) { // no device have been added! -> do not connect!
    _begin = false;
    DEBUG_SINRIC("[SinricPro]: ERROR! No valid devices available. Please add a valid device first!\r\n");
    return;
  }

  _websocketListener.begin(serverURL, socketAuthToken, deviceList, &receiveQueue);
}


void SinricProClass::stop() {
  DEBUG_SINRIC("[SinricPro:stop()\r\n");
  _websocketListener.stop();
}

bool SinricProClass::isConnected() {
  return _websocketListener.isConnected();
};


void SinricProClass::onConnected(ConnectCallbackHandler cb) {
  _websocketListener.onConnected(cb);
}

void SinricProClass::onDisconnected(ConnectCallbackHandler cb) {
  _websocketListener.onDisconnected(cb);
}


void SinricProClass::reconnect() {
  DEBUG_SINRIC("SinricPro:reconnect(): disconnecting\r\n");
  stop();
  DEBUG_SINRIC("SinricPro:reconnect(): connecting\r\n");
  connect();
}

bool SinricProClass::verifyDeviceId(const char* id) {
  if (strlen(id) != 24) return false;
  int tmp; char tmp_c;
  return sscanf(id, "%4x%4x%4x%4x%4x%4x%c", 
         &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp_c) == 6;
}

bool SinricProClass::verifyAppKey(const char* key) {
  if (strlen(key) != 36) return false;
  int tmp; char tmp_c;
  return sscanf(key, "%4x%4x-%4x-%4x-%4x-%4x%4x%4x%c",
         &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp_c) == 8;
}

bool SinricProClass::verifyAppSecret(const char* secret) {
  if (strlen(secret) != 73) return false;
  int tmp; char tmp_c;
  return sscanf(secret, "%4x%4x-%4x-%4x-%4x-%4x%4x%4x-%4x%4x-%4x-%4x-%4x-%4x%4x%4x%c",
         &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp_c) == 16;
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
    DEBUG_SINRIC("[SinricPro:extractTimestamp(): Got Timestamp %lu\r\n", tempTimestamp);
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
  payload["cause"].createNestedObject("type");
  payload["cause"]["type"] = cause;
  payload["createdAt"] = 0;
  payload["deviceId"] = deviceId;
  payload["replyToken"] = MessageID().getID();
  payload["type"] = "event";
  payload.createNestedObject("value");
  return eventMessage;
}

#ifndef NOSINRIC_INSTANCE
/**
 * @class SinricPro
 * @brief The main instance of SinricProClass
 * 
 * Handles communication between SinricPro Server and your device \n
  * @see SinricProClass
 **/
SinricProClass SinricPro;
#endif

#endif
