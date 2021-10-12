#pragma once

#include <ArduinoJson.h>
#include <WString.h>

#include <functional>

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

struct SinricProRequest {
    const String &action;
    const String &instance;
    JsonObject &  request_value;
    JsonObject &  response_value;
};

/**
 * @brief Abstract base class inherited by all capability classes that can handle requests.
 * 
 * @ingroup Capabilities
 */
class SinricProRequestHandler {
    friend class SinricProDevice;

  protected:
    virtual bool handleRequest(SinricProRequest& request) = 0;
};

}  // namespace SINRICPRO_NAMESPACE