#pragma once

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

#define FSTR(x, y) const char* FSTR_ ##x ##_ ##y PROGMEM = #y

FSTR(SINRICPRO, instanceId);
FSTR(SINRICPRO, message);
FSTR(SINRICPRO, success);
FSTR(SINRICPRO, response);
FSTR(SINRICPRO, header);
FSTR(SINRICPRO, payloadVersion);
FSTR(SINRICPRO, signatureVersion);
FSTR(SINRICPRO, payload);
FSTR(SINRICPRO, action);
FSTR(SINRICPRO, cause);
FSTR(SINRICPRO, type);
FSTR(SINRICPRO, createdAt);
FSTR(SINRICPRO, deviceId);
FSTR(SINRICPRO, replyToken);
FSTR(SINRICPRO, event);
FSTR(SINRICPRO, value);
FSTR(SINRICPRO, clientId);
FSTR(SINRICPRO, request);
FSTR(SINRICPRO, signature);
FSTR(SINRICPRO, HMAC);
FSTR(SINRICPRO, timestamp);
FSTR(SINRICPRO, PERIODIC_POLL);
FSTR(SINRICPRO, PHYSICAL_INTERACTION);

} // SINRICPRO_NAMESPACE