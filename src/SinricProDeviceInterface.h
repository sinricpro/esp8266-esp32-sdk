#pragma once

#include "SinricProInterface.h"
#include "SinricProRequest.h"
#include "SinricProId.h"

#include "SinricProNamespace.h"
namespace SINRICPRO_NAMESPACE {

class SinricProDeviceInterface {
  friend class SinricProClass;
  protected:
    virtual bool handleRequest(SinricProRequest &request) = 0;
    virtual const DeviceId getDeviceId() const = 0;
    virtual const String getProductType() const = 0;
    virtual const unsigned long getTimestamp() const = 0;
    virtual void loop() = 0;
};

} // SINRICPRO_NAMESPACE