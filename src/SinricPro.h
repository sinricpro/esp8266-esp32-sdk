#ifndef _SINRIC_H_
#define _SINRIC_H_

#include "EventSender.h"
#include "SinricProDevice.h"

#include "SinricProWebsocket.h"
#include "SinricProUDP.h"
#include "Globals.h"
#include "Signature.h"
#include "messageid.h"

#include "extralib/ESPTrueRandom/ESPTrueRandom.h"

class SinricProClass : public EventSender {
  public:
  void begin(String app_key, String app_secret, String server = SERVER_URL);
  template <typename DeviceType>
  DeviceType& add(const char* deviceId, unsigned long eventWaitTime = 1000);
  void add(SinricProDevice& newDevice);
  void add(SinricProDevice* newDevice);
  void handle();
  void stop();
  bool isConnected();

  //void sendEvent(JsonDocument& jsonEvent) override;
  DynamicJsonDocument prepareResponse(JsonDocument& requestMessage);
  DynamicJsonDocument prepareEvent(const char* deviceId, const char* action, const char* cause) override;
  void sendEvent(JsonDocument& event) override;
  private:
  void handleRequest();
  void handleSendQueue();
  void connect();
  void disconnect();
  void reconnect();

  std::vector<SinricProDevice*> devices;
  String _app_key;
  String _app_secret;
  String _server;

  websocketListener _websocketListener;
  udpListener _udpListener;
  myNTP _ntp;
};

void SinricProClass::begin(String app_key, String app_secret, String server) {
  _app_key = app_key;
  _app_secret = app_secret;
  _server = server;
  _ntp.begin();
}

template <typename DeviceType>
DeviceType& SinricProClass::add(const char* deviceId, unsigned long eventWaitTime) {
  DeviceType* newDevice = new DeviceType(deviceId, eventWaitTime);
  newDevice->begin(this);
  devices.push_back(newDevice);
  return *newDevice;
}

void SinricProClass::add(SinricProDevice* newDevice) {
  newDevice->begin(this);
  devices.push_back(newDevice);
}

void SinricProClass::add(SinricProDevice& newDevice) {
  add(&newDevice);
}

void SinricProClass::handle() {
  if (!isConnected()) connect();
  _websocketListener.handle();
  _udpListener.handle();
  _ntp.update();

  handleRequest();
  handleSendQueue();
}

void SinricProClass::handleRequest() {

  if (receiveQueue.count() == 0) return;
  
  DEBUG_SINRIC("[SinricProClass.handleRequest()]: %i requests in queue\r\n", receiveQueue.count());
  // POP requests and call device.handle() for each related device
  while (receiveQueue.count() > 0) {
    SinricProMessage* rawMessage = receiveQueue.pop();
    DynamicJsonDocument requestMessage(1024);
    deserializeJson(requestMessage, rawMessage->getMessage());
    // check signature
    bool sigMatch = verifyMessage(_app_secret, requestMessage);
    DEBUG_SINRIC("[SinricProClass.handleRequest(): Signature is %s\r\n", sigMatch?"valid":"invalid");

    if (sigMatch) { // signature is valid }
      #ifndef NODEBUG_SINRIC
        String debugOutput;
        serializeJsonPretty(requestMessage, debugOutput);
        DEBUG_SINRIC("%s\r\n", debugOutput.c_str());
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
      // sign message
      String responseString = signMessage(_app_secret.c_str(), responseMessage);
      // debug output message
      #ifndef NODEBUG_SINRIC
        String responseStringPretty;
        serializeJsonPretty(responseMessage, responseStringPretty);
        DEBUG_SINRIC("[SinricPro.handleRequest()]: response:\r\n%s\r\n", responseStringPretty.c_str());
      #endif
      // push response message to sendQueue
      sendQueue.push(new SinricProMessage(rawMessage->getInterface(), responseString.c_str()));
    } else { // signature is invalid!
//      DEBUG_SINRIC("[SinricProClass.handleRequest(): Signature should be: %s\r\n", calculateSignature(_app_secret.c_str(), jsonMessage).c_str());
    }
    delete rawMessage;
  }
}

void SinricProClass::handleSendQueue() {
  if (!isConnected()) return;
  if (sendQueue.count() > 0) {
    DEBUG_SINRIC("[SinricProClass:handleSendQueue]: %i send items in queue\r\n", sendQueue.count());
    SinricProMessage* rawMessage = sendQueue.pop();
    DEBUG_SINRIC("[SinricProClass:handleSendQueue]:\r\n%s\r\n", rawMessage->getMessage());
    switch (rawMessage->getInterface()) {
      case IF_WEBSOCKET: _websocketListener.sendResponse(rawMessage->getMessage()); break;
      case IF_UDP:       _udpListener.sendResponse(rawMessage->getMessage()); break;
      default:           break;
    }
    delete rawMessage;
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

    _websocketListener.begin(_server, _app_key, deviceList.c_str());
    while (_websocketListener.isConnected() == false) { DEBUG_SINRIC("."); delay(250); }
    DEBUG_SINRIC("\r\n");
    _udpListener.begin();
}


void SinricProClass::stop() {
  DEBUG_SINRIC("[SinricProClass:stop()\r\n");
  _websocketListener.disconnect();
  _websocketListener.stop();
}

bool SinricProClass::isConnected() {
  return _websocketListener.isConnected();
};


void SinricProClass::reconnect() {
  DEBUG_SINRIC("SinricProClass.reconnect(): disconnecting\r\n");
  disconnect();
  DEBUG_SINRIC("SinricProClass.reconnect(): wait 1second\r\n");
  delay(1000);
  DEBUG_SINRIC("SinricProClass.reconnect(): connecting\r\n");
  connect();
}


void SinricProClass::sendEvent(JsonDocument& event) {
  String messageString = signMessage(_app_secret, event);
  sendQueue.push(new SinricProMessage(IF_WEBSOCKET, messageString.c_str()));
  #ifndef NODEBUG_SINRIC
    String debugOutput;
    serializeJsonPretty(event, debugOutput);
    DEBUG_SINRIC("Signed event:\r\n%s\r\n", debugOutput.c_str());
  #endif
}


DynamicJsonDocument SinricProClass::prepareResponse(JsonDocument& requestMessage) {
  DynamicJsonDocument responseMessage(1024);
  JsonObject header = responseMessage.createNestedObject("header");
  header["payloadVersion"] = 2;
  header["signatureVersion"] = 1;

  JsonObject payload = responseMessage.createNestedObject("payload");
  payload["action"] = requestMessage["payload"]["action"];
  payload["clientId"] = requestMessage["payload"]["clientId"];
  payload["createdAt"] = _ntp.getTimestamp();
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
  payload["createdAt"] = _ntp.getTimestamp();
  payload["deviceId"] = deviceId;
  MessageID msgId;
  payload["replyToken"] = msgId.getID();
  payload["type"] = "event";
  payload.createNestedObject("value");
  return eventMessage;
}


SinricProClass SinricPro;

#endif