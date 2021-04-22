/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SIGNATURE_H_
#define _SIGNATURE_H_

String HMACbase64(const String &message, const String &key);
String extractPayload(const char *message);
String calculateSignature(const char* key, String payload);
String signMessage(String key, JsonDocument &jsonMessage);

#endif // _SIGNATURE_H_