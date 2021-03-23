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
#if !defined(SINRICPRO_OO)    
    virtual void begin(SinricProInterface* eventSender) = 0;
#endif    
    virtual unsigned long getTimestamp();
};

#endif