#pragma once

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

/**
 * @brief FSTR(x, y) Helper macro to define const char* by a given structure
 * Example:
 *   FSTR(SINRICPRO, test);
 * results in 
 *   const char* FSTR_SINRICPRO_test PROGMEM = "test";
 * 
 */
#define FSTR(x, y) const char* FSTR_ ##x ##_ ##y PROGMEM = #y

FSTR(SINRICPRO, instanceId);              // "instanceId"
FSTR(SINRICPRO, message);                 // "message"
FSTR(SINRICPRO, success);                 // "success"
FSTR(SINRICPRO, response);                // "response"
FSTR(SINRICPRO, header);                  // "header"
FSTR(SINRICPRO, payloadVersion);          // "payloadVersion"
FSTR(SINRICPRO, signatureVersion);        // "signatureVersion"
FSTR(SINRICPRO, payload);                 // "payload"
FSTR(SINRICPRO, action);                  // "action"
FSTR(SINRICPRO, cause);                   // "cause"
FSTR(SINRICPRO, type);                    // "type"
FSTR(SINRICPRO, createdAt);               // "createdAt"
FSTR(SINRICPRO, deviceId);                // "deviceId"
FSTR(SINRICPRO, replyToken);              // "replyToken"
FSTR(SINRICPRO, event);                   // "event"
FSTR(SINRICPRO, value);                   // "value"
FSTR(SINRICPRO, clientId);                // "clientId"
FSTR(SINRICPRO, request);                 // "request"
FSTR(SINRICPRO, signature);               // "signature"
FSTR(SINRICPRO, HMAC);                    // "HMAC"
FSTR(SINRICPRO, timestamp);               // "timestamp"
FSTR(SINRICPRO, PERIODIC_POLL);           // "PERIODIC_POLL"
FSTR(SINRICPRO, PHYSICAL_INTERACTION);    // "PHYSICAL_INTERACTION"
FSTR(SINRICPRO, ALERT);                   // "ALERT"
FSTR(SINRICPRO, OK);                      // "OK"

} // SINRICPRO_NAMESPACE