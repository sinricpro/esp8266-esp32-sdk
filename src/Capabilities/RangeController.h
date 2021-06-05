#pragma once

#include "../SinricProRequest.h"
#include "../EventLimiter.h"

#include "../SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

using GenericRangeValueCallback_int   = bool (*)(const String &, const String &, int &);
using GenericRangeValueCallback_float = bool (*)(const String &, const String &, float &);

struct GenericRangeValueCallback {
  GenericRangeValueCallback() : type(type_unknown) {}
  GenericRangeValueCallback(GenericRangeValueCallback_int cb)   : type(type_int), cb_int(cb) {}
  GenericRangeValueCallback(GenericRangeValueCallback_float cb) : type(type_float), cb_float(cb) {}
  enum {
    type_unknown,
    type_int,
    type_float
  } type;
  union {
    GenericRangeValueCallback_int   cb_int;
    GenericRangeValueCallback_float cb_float;
  };
};

using SetRangeValueCallback                 = std::function<bool(const String &, int &)>;
using AdjustRangeValueCallback              = std::function<bool(const String &, int &)>;

using GenericSetRangeValueCallback_int      = GenericRangeValueCallback_int;
using GenericSetRangeValueCallback_float    = GenericRangeValueCallback_float;
using GenericAdjustRangeValueCallback_int   = GenericRangeValueCallback_int;
using GenericAdjustRangeValueCallback_float = GenericRangeValueCallback_float;


template <typename T>
class RangeController {
  public:
    RangeController();

    void onRangeValue(SetRangeValueCallback cb);
    void onRangeValue(const String& instance, GenericSetRangeValueCallback_int cb);
    void onRangeValue(const String& instance, GenericSetRangeValueCallback_float cb);
    void onAdjustRangeValue(AdjustRangeValueCallback cb);
    void onAdjustRangeValue(const String& instance, GenericAdjustRangeValueCallback_int cb);
    void onAdjustRangeValue(const String& instance, GenericAdjustRangeValueCallback_float cb);

    bool sendRangeValueEvent(int rangeValue, String cause = "PHYSICAL_INTERACTION");
    bool sendRangeValueEvent(const String &instance, int rangeValue, String cause = "PHYSICAL_INTERACTION");
    bool sendRangeValueEvent(const String &instance, float rangeValue, String cause = "PHYSICAL_INTERACTION");

  protected:
    virtual bool onRangeValue(int &value);
    virtual bool onRangeValue(const String &instance, int &value);
    virtual bool onRangeValue(const String &instance, float &value);
    virtual bool onAdjustRangeValue(int &value);
    virtual bool onAdjustRangeValue(const String &instance, int &value);
    virtual bool onAdjustRangeValue(const String &instance, float &value);

    bool handleRangeController(SinricProRequest &request);
  
  private:
    EventLimiter event_limiter;
    std::map<String, EventLimiter> generic_event_limiter;
    SetRangeValueCallback setRangeValueCallback;
    std::map<String, GenericRangeValueCallback> genericSetRangeValueCallback;
    AdjustRangeValueCallback adjustRangeValueCallback;
    std::map<String, GenericRangeValueCallback> genericAdjustRangeValueCallback;
};


template <typename T>
RangeController<T>::RangeController()
: event_limiter(EVENT_LIMIT_STATE) { 
  T* device = static_cast<T*>(this);
  device->registerRequestHandler(std::bind(&RangeController<T>::handleRangeController, this, std::placeholders::_1)); 
}

template <typename T>
void RangeController<T>::onRangeValue(SetRangeValueCallback cb) {
  setRangeValueCallback = cb;
}

template <typename T>
void RangeController<T>::onRangeValue(const String& instance, GenericSetRangeValueCallback_int cb) {
  genericSetRangeValueCallback[instance] = GenericRangeValueCallback(cb);
}

template <typename T>
void RangeController<T>::onRangeValue(const String& instance, GenericSetRangeValueCallback_float cb) {
  genericSetRangeValueCallback[instance] = GenericRangeValueCallback(cb);
}

template <typename T>
void RangeController<T>::onAdjustRangeValue(AdjustRangeValueCallback cb) {
  adjustRangeValueCallback = cb;
}

template <typename T>
void RangeController<T>::onAdjustRangeValue(const String& instance, GenericAdjustRangeValueCallback_int cb) {
  genericAdjustRangeValueCallback[instance] = GenericRangeValueCallback(cb);
}

template <typename T>
void RangeController<T>::onAdjustRangeValue(const String& instance, GenericAdjustRangeValueCallback_float cb) {
  genericAdjustRangeValueCallback[instance] = GenericRangeValueCallback(cb);
}

template <typename T>
bool RangeController<T>::onRangeValue(int &value) {
  T* device = static_cast<T*>(this);
  if (setRangeValueCallback) setRangeValueCallback(device->deviceId, value);
  return false;
}

template <typename T>
bool RangeController<T>::onRangeValue(const String &instance, int &value) {
  T* device = static_cast<T*>(this);
  if (genericSetRangeValueCallback.find(instance) == genericAdjustRangeValueCallback.end()) return false;
  if (genericSetRangeValueCallback[instance].type == GenericRangeValueCallback::type_float) return false;
  return genericSetRangeValueCallback[instance].cb_int(device->deviceId, instance, value);
}

template <typename T>
bool RangeController<T>::onRangeValue(const String &instance, float &value) {
  T* device = static_cast<T*>(this);
  if (genericSetRangeValueCallback.find(instance) == genericAdjustRangeValueCallback.end()) return false;
  if (genericSetRangeValueCallback[instance].type == GenericRangeValueCallback::type_int) return false;
  return genericSetRangeValueCallback[instance].cb_float(device->deviceId, instance, value);
}

template <typename T>
bool RangeController<T>::onAdjustRangeValue(int &value) {
  T* device = static_cast<T*>(this);
  if (adjustRangeValueCallback) return adjustRangeValueCallback(device->deviceId, value);
  return false;
}

template <typename T>
bool RangeController<T>::onAdjustRangeValue(const String &instance, int &value) {
  T* device = static_cast<T*>(this);
  if (genericAdjustRangeValueCallback.find(instance) == genericAdjustRangeValueCallback.end()) return false;
  if (genericAdjustRangeValueCallback[instance].type == GenericRangeValueCallback::type_float) return false;
  return genericAdjustRangeValueCallback[instance].cb_int(device->deviceId, instance, value);
}

template <typename T>
bool RangeController<T>::onAdjustRangeValue(const String &instance, float &value) {
  T* device = static_cast<T*>(this);
  if (genericAdjustRangeValueCallback.find(instance) == genericAdjustRangeValueCallback.end()) return false;
  if (genericAdjustRangeValueCallback[instance].type == GenericRangeValueCallback::type_int) return false;
  return genericAdjustRangeValueCallback[instance].cb_float(device->deviceId, instance, value);
}

template <typename T>
bool RangeController<T>::sendRangeValueEvent(int rangeValue, String cause) {
  if (event_limiter) return false;
  T &device = static_cast<T &>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setRangeValue", cause.c_str());
  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool RangeController<T>::sendRangeValueEvent(const String &instance, int rangeValue, String cause) {
  if (generic_event_limiter.find(instance) == generic_event_limiter.end()) generic_event_limiter[instance] = EventLimiter(EVENT_LIMIT_STATE);
  if (generic_event_limiter[instance]) return false;
  T &device = static_cast<T &>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setRangeValue", cause.c_str());
  eventMessage["payload"]["instanceId"] = instance;

  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool RangeController<T>::sendRangeValueEvent(const String &instance, float rangeValue, String cause) {
  if (generic_event_limiter.find(instance) == generic_event_limiter.end()) generic_event_limiter[instance] = EventLimiter(EVENT_LIMIT_STATE);
  if (generic_event_limiter[instance]) return false;
  T &device = static_cast<T &>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setRangeValue", cause.c_str());
  eventMessage["payload"]["instanceId"] = instance;

  JsonObject event_value = eventMessage["payload"]["value"];
  event_value["rangeValue"] = rangeValue;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool RangeController<T>::handleRangeController(SinricProRequest &request) {
  bool success = false;

  if (request.action == "setRangeValue") {

    if (request.instance == "") {

      int value = request.request_value["rangeValue"];
      success = onRangeValue(value);
      request.response_value["rangeValue"] = value;
      return success;

    } else {

      if (request.request_value["rangeValue"].is<int>()) {

        int value = request.request_value["rangeValue"];
        success = onRangeValue(request.instance, value);
        request.response_value["rangeValue"] = value;
        return success;

      } else {

        float value = request.request_value["rangeValue"];
        success = onRangeValue(request.instance, value);
        request.response_value["rangeValue"] = value;
        return success;

      }
    }
  }

  if (request.action == "adjustRangeValue") {

    if (request.instance == "") {

      int value = request.request_value["rangeValueDelta"];
      success = onAdjustRangeValue(value);
      request.response_value["rangeValue"] = value;
      return success;
    
    } else {

      if (request.request_value["rangeValueDelta"].is<int>()) {

        int value = request.request_value["rangeValueDelta"];
        success = onRangeValue(request.instance, value);
        request.response_value["rangeValue"] = value;
        return success;

      } else {

        float value = request.request_value["rangeValueDelta"];
        success = onRangeValue(request.instance, value);
        request.response_value["rangeValue"] = value;
        return success;
      }
    }
  }

  return false;
}

} // SINRICPRO_NAMESPACE