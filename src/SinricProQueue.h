/*
 *  Copyright (c) 2021 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include <Arduino.h>
#include <queue>

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

typedef enum {
  IF_UNKNOWN    = 0,
  IF_WEBSOCKET  = 1,
  IF_UDP        = 2
} interface_t;

class SinricProMessage {
public:
  SinricProMessage(interface_t interface, const char* message);
  SinricProMessage(interface_t interface, const char* message, const IPAddress& remoteIP, uint16_t remotePort);
  ~SinricProMessage();
  const char*      getMessage() const;
  interface_t      getInterface() const;
  const IPAddress& getRemoteIP() const;
  uint16_t         getRemotePort() const;
private:
  interface_t   _interface;
  char*         _message;
  IPAddress     _remoteIP;
  uint16_t      _remotePort = 0;
};

SinricProMessage::SinricProMessage(interface_t interface, const char* message) : 
  _interface(interface) { 
  _message = strdup(message); 
};

SinricProMessage::SinricProMessage(interface_t interface, const char* message, const IPAddress& remoteIP, uint16_t remotePort) :
  _interface(interface), _remoteIP(remoteIP), _remotePort(remotePort) {
  _message = strdup(message);
};

SinricProMessage::~SinricProMessage() { 
  if (_message) free(_message); 
};

const char* SinricProMessage::getMessage() const { 
  return _message; 
};

interface_t SinricProMessage::getInterface() const { 
  return _interface; 
};

const IPAddress& SinricProMessage::getRemoteIP() const {
  return _remoteIP;
};

uint16_t SinricProMessage::getRemotePort() const {
  return _remotePort;
};


typedef std::queue<SinricProMessage*> SinricProQueue_t;

} // SINRICPRO_NAMESPACE