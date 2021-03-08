#ifndef _SINRICPROREQUEST_H_
#define _SINRICPROREQUEST_H_

#include <WString.h>
#include <ArduinoJson.h>
#include <functional>

struct SinricProRequest {
  const String &action;
  const String &instance;
  JsonObject &request_value;
  JsonObject &response_value;
};

using SinricProRequestHandler = std::function<bool(SinricProRequest&)>;

#endif