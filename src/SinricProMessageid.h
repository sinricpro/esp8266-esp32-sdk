/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

class MessageID {
public:
  MessageID();
  const String& getID() { return _id; }
private:
  String _id;
};

MessageID::MessageID() {
  _id = "";
  for (byte i=0; i<16; i++) {
    byte rnd = random(255);
    if (i==4) _id += "-";
    if (i==6) { _id += "-"; rnd = 0x40 | (0x0F & rnd); } // 0100xxxx to set version 4
    if (i==8) { _id += "-"; rnd = 0x80 | (0x3F & rnd); } // 10xxxxxx to set reserved bits
    if (i==10) _id += "-";
    byte high_nibble = rnd >> 4;
    byte low_nibble = rnd & 0x0f;
    _id += "0123456789abcdef"[high_nibble];
    _id += "0123456789abcdef"[low_nibble];
  }
}

} // SINRICPRO_NAMESPACE