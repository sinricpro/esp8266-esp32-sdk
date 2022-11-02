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

class SinricProRequestHandler {
  friend class SinricProDevice;
  private:
    virtual bool handleRequest(SinricProRequest&) = 0;
};

} // SINRICPRO_NAMESPACE