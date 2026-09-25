/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include <Arduino.h>

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

// Standard base64 (RFC 4648) on one line, with padding.
String base64Encode(const uint8_t *data, size_t length);

// Returns false for empty input, a length that is not a multiple of 4, or characters outside the
// base64 alphabet. libb64 would otherwise skip them silently and decode whatever remains.
bool base64Decode(const char *input, String &output);

} // SINRICPRO_NAMESPACE
