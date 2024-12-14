#pragma once

#include "SinricProInterface.h"
#include "SinricProNamespace.h"
#include "SinricProRequest.h"
namespace SINRICPRO_NAMESPACE {

class SinricProDeviceInterface {
    friend class SinricProClass;

  protected:
    virtual bool          handleRequest(SinricProRequest& request) = 0;
    virtual String        getDeviceId()                            = 0;
    virtual String        getProductType()                         = 0;
    virtual void          begin(SinricProInterface* eventSender)   = 0;
    virtual unsigned long getTimestamp()                           = 0;
};

}  // namespace SINRICPRO_NAMESPACE