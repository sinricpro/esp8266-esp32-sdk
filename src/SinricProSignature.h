/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef _SIGNATURE_H_
#define _SIGNATURE_H_

#if defined (ESP8266)
  #include <bearssl/bearssl_hmac.h>
  #include <base64.h>
#endif
#if defined (ESP32)
  #include "mbedtls/md.h"
  #include "mbedtls/base64.h"
#endif


String HMACbase64(const String &message, const String &key) {
  byte hmacResult[32];

#if defined(ESP8266)
  br_hmac_key_context keyContext; // Holds general HMAC info
  br_hmac_context hmacContext;    // Holds general HMAC info + specific info for the current operation

  br_hmac_key_init(&keyContext, &br_sha256_vtable, key.c_str(), key.length());
  br_hmac_init(&hmacContext, &keyContext, 32);
  br_hmac_update(&hmacContext, message.c_str(), message.length());
  br_hmac_out(&hmacContext, hmacResult);

  base64 encoded;
  return encoded.encode((const uint8_t *) hmacResult, 32, false);
#endif

#if defined(ESP32)
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
  mbedtls_md_hmac_starts(&ctx, (const unsigned char*) key.c_str(), key.length());
  mbedtls_md_hmac_update(&ctx, (const unsigned char*) message.c_str(), message.length());
  mbedtls_md_hmac_finish(&ctx, hmacResult);
  mbedtls_md_free(&ctx);

  unsigned char encoded[100];
  size_t outputLen;
  mbedtls_base64_encode(encoded, 100, &outputLen, (const unsigned char *) hmacResult, 32);
  return String{ (char*) encoded};

#endif
}

String calculateSignature(const char* key, JsonDocument &jsonMessage) {
  if (!jsonMessage.containsKey("payload")) return String("");
  String jsonPayload; serializeJson(jsonMessage["payload"], jsonPayload);
  return HMACbase64(jsonPayload, String(key));
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