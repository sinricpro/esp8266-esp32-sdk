#ifndef _SINRICPRODEVICEINTERFACE_
#define _SINRICPRODEVICEINTERFACE_

#include <SinricProInterface.h>
#include <SinricProId.h>

class SinricProDeviceInterface {
  public:
    virtual bool handleRequest(const DeviceId &deviceId, const char* action, JsonObject &request_value, JsonObject &response_value) = 0;
    virtual DeviceId getDeviceId() = 0;
    virtual String getProductType() = 0;
    virtual void begin(SinricProInterface* eventSender) = 0;
  protected:
    virtual bool sendEvent(JsonDocument& event) = 0;
    virtual DynamicJsonDocument prepareEvent(const DeviceId &deviceId, const char* action, const char* cause) = 0;
};

#endif