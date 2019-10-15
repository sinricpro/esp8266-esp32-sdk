/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef __SINRICPRO_QUEUE_H__
#define __SINRICPRO_QUEUE_H__

#include "extralib/QueueList/QueueList.h"


typedef enum {
  IF_UNKNOWN    = 0,
  IF_WEBSOCKET  = 1,
  IF_UDP        = 2
} interface_t;

class SinricProMessage {
public:
  SinricProMessage(interface_t interface, const char* message) : _interface(interface) { _message = strdup(message); };
  ~SinricProMessage() { if (_message) free(_message); };
  const char* getMessage() { return _message; };
  interface_t getInterface() { return _interface; };
private:
  interface_t _interface;
  char* _message;
};

typedef QueueList<SinricProMessage*> SinricProQueue_t;

#endif
