#ifndef _SINRICPRODEVICEINTERFACE_
#define _SINRICPRODEVICEINTERFACE_

#include <SinricProInterface.h>
#include <SinricProRequest.h>
#include <SinricProId.h>

class SinricProDeviceInterface {
  friend class SinricProClass;
  protected:
    virtual bool handleRequest(SinricProRequest &request) = 0;
    virtual DeviceId getDeviceId() = 0;
    virtual String getProductType() = 0;
    virtual void begin(SinricProInterface* eventSender) = 0;
//    virtual bool sendEvent(JsonDocument& event) = 0;
//    virtual DynamicJsonDocument prepareEvent(const char* action, const char* cause) = 0;
    virtual unsigned long getTimestamp();
};

#endif