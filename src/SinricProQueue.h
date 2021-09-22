/*
 *  Copyright (c) 2021 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

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
  ~SinricProMessage();
  const char*   getMessage() const;
  interface_t   getInterface() const;
private:
  interface_t   _interface;
  char*         _message;
};

SinricProMessage::SinricProMessage(interface_t interface, const char* message) : 
  _interface(interface) { 
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


typedef std::queue<SinricProMessage*> SinricProQueue_t;

} // SINRICPRO_NAMESPACE