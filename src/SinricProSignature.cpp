/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#include <WString.h>
#include <ArduinoJson.h>
#include "SinricProSignature.h"

#if defined (ESP8266)
  #include <bearssl/bearssl_hmac.h>
#endif
#if defined (ESP32)
  #include "mbedtls/md.h"
#endif
  #include <libb64/cencode.h>
  
#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

String HMACbase64(const String &message, const String &key) {
  byte hmacResult[32];
#if defined(ESP8266)
  br_hmac_key_context keyContext; // Holds general HMAC info
  br_hmac_context hmacContext;    // Holds general HMAC info + specific info for the current operation

  br_hmac_key_init(&keyContext, &br_sha256_vtable, key.c_str(), key.length());
  br_hmac_init(&hmacContext, &keyContext, 32);
  br_hmac_update(&hmacContext, message.c_str(), message.length());
  br_hmac_out(&hmacContext, hmacResult);
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

#endif

  base64_encodestate _state;
  base64_init_encodestate(&_state);
#if defined(base64_encode_expected_len_nonewlines)
  _state.stepsnewline = -1;
#endif  
  char base64encodedHMAC[base64_encode_expected_len(32) + 1];
  int len = base64_encode_block((const char *)hmacResult, 32, base64encodedHMAC, &_state);
  base64_encode_blockend((base64encodedHMAC + len), &_state);
  
  return String { base64encodedHMAC };
}

String extractPayload(const char *message) {
  String messageStr(message);
  int beginPayload = messageStr.indexOf("\"payload\":");
  int endPayload = messageStr.indexOf(",\"signature\"", beginPayload);
  if (beginPayload >0 && endPayload >0) return messageStr.substring(beginPayload+10, endPayload);
  return "";
}

String calculateSignature(const char* key, String payload) {
  if (payload != "") return HMACbase64(payload, String(key));
  return "";
}

String signMessage(String key, JsonDocument &jsonMessage) {
  if (!jsonMessage.containsKey("signature")) jsonMessage.createNestedObject("signature");
  jsonMessage["signature"]["HMAC"] = calculateSignature(key.c_str(), jsonMessage["payload"]);
  String signedMessageString;
  serializeJson(jsonMessage, signedMessageString);
  return signedMessageString;
}

} // SINRICPRO_NAMESPACE