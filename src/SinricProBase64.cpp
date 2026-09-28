/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#include "SinricProBase64.h"

#include <ctype.h>
#include <vector>
#include <libb64/cdecode.h>
#include <libb64/cencode.h>

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

String base64Encode(const uint8_t *data, size_t length) {
  base64_encodestate state;
  base64_init_encodestate(&state);
#if defined(base64_encode_expected_len_nonewlines)
  // The ESP8266 and RP2040 builds of libb64 insert a newline every 72 characters unless told not to.
  state.stepsnewline = -1;
#endif
  // Zero-filled, so the result ends at the first NUL whichever libb64 variant wrote it.
  std::vector<char> buffer(base64_encode_expected_len(length) + 1, 0);
  int written = base64_encode_block(reinterpret_cast<const char *>(data), length, buffer.data(), &state);
  base64_encode_blockend(buffer.data() + written, &state);
  return String(buffer.data());
}

bool base64Decode(const char *input, String &output) {
  const size_t length = strlen(input);
  if (length == 0 || length % 4 != 0) return false;

  for (size_t i = 0; i < length; ++i) {
    const unsigned char c = static_cast<unsigned char>(input[i]);
    if (c == '=') {
      // Padding: at most two characters, and only at the end.
      if (i < length - 2 || (i == length - 2 && input[length - 1] != '=')) return false;
    } else if (!isalnum(c) && c != '+' && c != '/') {
      return false;
    }
  }

  std::vector<char> buffer(length / 4 * 3 + 1, 0);
  base64_decodestate state;
  base64_init_decodestate(&state);
  const int decoded = base64_decode_block(input, length, buffer.data(), &state);

  output = "";
  return output.concat(buffer.data(), decoded);
}

} // SINRICPRO_NAMESPACE
