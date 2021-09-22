/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

String HMACbase64(const String &message, const String &key);
String extractPayload(const char *message);
String calculateSignature(const char* key, String payload);
String signMessage(String key, JsonDocument &jsonMessage);

} // SINRICPRO_NAMESPACE