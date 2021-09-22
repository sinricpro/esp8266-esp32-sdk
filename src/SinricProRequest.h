#pragma once

#include <WString.h>
#include <ArduinoJson.h>
#include <functional>

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

struct SinricProRequest {
  const String &action;
  const String &instance;
  JsonObject &request_value;
  JsonObject &response_value;
};

using SinricProRequestHandler = std::function<bool(SinricProRequest&)>;

} // SINRICPRO_NAMESPACE