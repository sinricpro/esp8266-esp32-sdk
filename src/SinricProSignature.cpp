/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#include <WString.h>
#include <ArduinoJson.h>
#include "SinricProSignature.h"
#include "SinricProBase64.h"
#include "SinricProDebug.h"

#if defined (ESP8266) || defined(ARDUINO_ARCH_RP2040)
  #include <bearssl/bearssl_hmac.h>
  #define SINRICPRO_HMAC_BEARSSL
#elif defined (ESP32)
  #include <mbedtls/build_info.h>
  // Mbed TLS 4 (ESP-IDF 6, arduino-esp32 4.x) dropped HMAC from the MD API; it is PSA-only.
  #if MBEDTLS_VERSION_MAJOR >= 4
    #include <psa/crypto.h>
    #define SINRICPRO_HMAC_PSA
  #else
    #include <mbedtls/md.h>
    #define SINRICPRO_HMAC_MBEDTLS_MD
  #endif
#endif

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

String HMACbase64(const String &message, const String &key) {
  byte hmacResult[32] = {0};
#if defined(SINRICPRO_HMAC_BEARSSL)
  br_hmac_key_context keyContext; // Holds general HMAC info
  br_hmac_context hmacContext;    // Holds general HMAC info + specific info for the current operation

  br_hmac_key_init(&keyContext, &br_sha256_vtable, key.c_str(), key.length());
  br_hmac_init(&hmacContext, &keyContext, 32);
  br_hmac_update(&hmacContext, message.c_str(), message.length());
  br_hmac_out(&hmacContext, hmacResult);

#elif defined(SINRICPRO_HMAC_PSA)
  const psa_algorithm_t alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
  psa_set_key_algorithm(&attributes, alg);
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);

  mbedtls_svc_key_id_t keyId = MBEDTLS_SVC_KEY_ID_INIT;
  size_t macLength = 0;
  // psa_crypto_init() is idempotent and must precede any PSA call.
  if (psa_crypto_init() == PSA_SUCCESS &&
      psa_import_key(&attributes, (const uint8_t*) key.c_str(), key.length(), &keyId) == PSA_SUCCESS) {
    if (psa_mac_compute(keyId, alg, (const uint8_t*) message.c_str(), message.length(),
                        hmacResult, sizeof(hmacResult), &macLength) != PSA_SUCCESS) {
      DEBUG_SINRIC("[SinricPro:Signature]: psa_mac_compute failed\r\n");
    }
    psa_destroy_key(keyId);
  } else {
    DEBUG_SINRIC("[SinricPro:Signature]: PSA HMAC key import failed\r\n");
  }
  psa_reset_key_attributes(&attributes);

#elif defined(SINRICPRO_HMAC_MBEDTLS_MD)
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
  mbedtls_md_hmac_starts(&ctx, (const unsigned char*) key.c_str(), key.length());
  mbedtls_md_hmac_update(&ctx, (const unsigned char*) message.c_str(), message.length());
  mbedtls_md_hmac_finish(&ctx, hmacResult);
  mbedtls_md_free(&ctx);
#endif

  return base64Encode(hmacResult, sizeof(hmacResult));
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
  if (!jsonMessage["signature"].is<String>()) jsonMessage["signature"].to<JsonObject>();
  jsonMessage["signature"]["HMAC"] = calculateSignature(key.c_str(), jsonMessage["payload"]);
  String signedMessageString;
  serializeJson(jsonMessage, signedMessageString);
  return signedMessageString;
}

} // SINRICPRO_NAMESPACE