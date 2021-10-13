#pragma once

#include <Arduino.h>
#include <SinricProSwitch.h>

class MySwitchButton : public SinricProSwitch {
  public:
    MySwitchButton(const String& deviceId, int switch_pin, int button_pin);
    virtual bool onPowerState(bool& state) override;
    virtual void loop() override;

    void toggle();

  protected:
    int switch_pin;
    int button_pin;

    unsigned long lastButtonPress;
    bool          lastButtonState;
};

MySwitchButton::MySwitchButton(const String& deviceId, int switch_pin, int button_pin)
    : SinricProSwitch(deviceId)
    , switch_pin(switch_pin)
    , button_pin(button_pin) {
    pinMode(switch_pin, OUTPUT);
    pinMode(button_pin, INPUT);
}

bool MySwitchButton::onPowerState(bool& state) {
    digitalWrite(switch_pin, state);
    return true;
}

void MySwitchButton::toggle() {
    bool newRelayState = !digitalRead(switch_pin);
    digitalWrite(switch_pin, newRelayState);
    sendPowerStateEvent(newRelayState);
}

void MySwitchButton::loop() {
    unsigned long currentMillis      = millis();
    bool          currentButtonState = digitalRead(button_pin);

    if (currentButtonState != lastButtonState && currentMillis - lastButtonPress >= 50) {
        lastButtonPress = currentMillis;
        lastButtonState = currentButtonState;
        if (currentButtonState) toggle();
    }
}