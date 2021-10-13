#pragma once

#include <Arduino.h>
#include <SinricProSwitch.h>

class MySwitch : public SinricProSwitch {
  public:
    MySwitch(const String& deviceId, int switch_pin);
    virtual bool onPowerState(bool& state) override;

  protected:
    int switch_pin;
};

MySwitch::MySwitch(const String& deviceId, int switch_pin)
    : SinricProSwitch(deviceId)
    , switch_pin(switch_pin) {
    pinMode(switch_pin, OUTPUT);
}

bool MySwitch::onPowerState(bool& state) {
    digitalWrite(switch_pin, state);
    return true;
}
