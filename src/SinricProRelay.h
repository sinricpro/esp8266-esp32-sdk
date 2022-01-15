#pragma once

#include <Arduino.h>

#include "SinricProNamespace.h"
#include "SinricProSwitch.h"

namespace SINRICPRO_NAMESPACE {

class Relay : protected SinricProSwitch {
  public:
    Relay(const String& deviceId, int gpio, bool inverted = false);

    void turnOn(bool sendEvent = false);
    void turnOff(bool sendEvent = false);

  protected:
    int  gpio;
    bool inverted;

    virtual bool onPowerState(bool& state) override;
};

Relay::Relay(const String& deviceId, int gpio, bool inverted)
    : SinricProSwitch(deviceId), gpio(gpio), inverted(inverted) {
    pinMode(gpio, OUTPUT);
};

void Relay::turnOn(bool sendEvent) {
    bool oldState = inverted ? !digitalRead(gpio) : digitalRead(gpio);
    if (oldState) return;

    digitalWrite(gpio, inverted ? LOW : HIGH);
    if (sendEvent) sendPowerStateEvent(true);
}

void Relay::turnOff(bool sendEvent) {
    bool oldState = inverted ? !digitalRead(gpio) : digitalRead(gpio);
    if (!oldState) return;

    digitalWrite(gpio, inverted ? HIGH : LOW);
    if (sendEvent) sendPowerStateEvent(false);
}

bool Relay::onPowerState(bool& state) {
    digitalWrite(gpio, inverted ? !state : state);
    return true;
}

}  // namespace SINRICPRO_NAMESPACE

using SinricProRelay = SINRICPRO_NAMESPACE::Relay;