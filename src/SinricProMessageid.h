/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef __MESSAGEID_H__
#define __MESSAGEID_H__

#include "extralib/ESPTrueRandom/ESPTrueRandom.h"

class MessageID {
public:
  MessageID();
  char* getID() { return msg_id; };
private:
  void newMsgId();
  char msg_id[37];
};

MessageID::MessageID() {
  newMsgId();
}

void MessageID::newMsgId() {
  byte new_uuid[16];
  ESPTrueRandom.uuid(new_uuid);
  strlcpy(msg_id, ESPTrueRandom.uuidToString(new_uuid).c_str(), 37);
}

#endif // __MESSAGEID_H__
