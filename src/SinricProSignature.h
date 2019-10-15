/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SIGNATURE_H_
#define _SIGNATURE_H_

#include "extralib/Crypto/Crypto.h"
#include "extralib/Crypto/Base64.h"

String calculateSignature(const char* key, JsonDocument &jsonMessage) {
  if (!jsonMessage.containsKey("payload")) return String("");
  String jsonPayload; serializeJson(jsonMessage["payload"], jsonPayload);

  byte rawSigBuf[SHA256HMAC_SIZE];
  String test;
  

  SHA256HMAC hmac((byte*) key, strlen(key));
  hmac.doUpdate(jsonPayload.c_str(), jsonPayload.length());
  hmac.doFinal(rawSigBuf);
  

  int b64_len = base64_enc_len(SHA256HMAC_SIZE);
  char sigBuf[b64_len+1];
  base64_encode(sigBuf, (char*) rawSigBuf, SHA256HMAC_SIZE);
  sigBuf[b64_len] = 0;
  String result = sigBuf;

  return result;
}

bool verifyMessage(String key, JsonDocument &jsonMessage) {
  String jsonHash = jsonMessage["signature"]["HMAC"];
  String calculatedHash = calculateSignature(key.c_str(), jsonMessage);
  return jsonHash == calculatedHash;
}

String signMessage(String key, JsonDocument &jsonMessage) {
  if (!jsonMessage.containsKey("signature")) jsonMessage.createNestedObject("signature");
  jsonMessage["signature"]["HMAC"] = calculateSignature(key.c_str(), jsonMessage);
  String signedMessageString;
  serializeJson(jsonMessage, signedMessageString);
  return signedMessageString;
}

#endif // _SIGNATURE_H_