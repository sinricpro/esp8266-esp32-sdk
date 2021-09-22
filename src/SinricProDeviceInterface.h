#pragma once

#include "SinricProInterface.h"
#include "SinricProRequest.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

class SinricProDeviceInterface {
  friend class SinricProClass;
  protected:
    virtual bool          handleRequest(SinricProRequest &request) = 0;
    virtual String        getDeviceId() = 0;
    virtual String        getProductType() = 0;
    virtual void          begin(SinricProInterface* eventSender) = 0;
    virtual unsigned long getTimestamp();
};

} // SINRICPRO_NAMESPACE