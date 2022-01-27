#pragma once

#include <ArduinoJson.h>
#include <WString.h>

#include <functional>

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

class SinricProRequest {
  public:
    SinricProRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value);
    const String &action;
    const String &instance;
    JsonObject &  request_value;
    JsonObject &  response_value;
};

SinricProRequest::SinricProRequest(const String &action, const String &instance, JsonObject &request_value, JsonObject &response_value)
    : action(action)
    , instance(instance)
    , request_value(request_value)
    , response_value(response_value) {}

/**
 * @brief Abstract base class inherited by all capability classes that can handle requests.
 * 
 * @ingroup Capabilities
 */
class SinricProRequestHandler {
    friend class SinricProDevice;

  protected:
    virtual bool handleRequest(SinricProRequest &request) = 0;
};

}  // namespace SINRICPRO_NAMESPACE