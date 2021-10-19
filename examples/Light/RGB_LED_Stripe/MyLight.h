#include <SinricProLight.h>

class MyLight : public SinricProLight {
  public:
    MyLight(const String& deviceId, int pin_r, int pin_g, int pin_b);

    bool onPowerState(bool& powerState) override;
    bool onBrightness(int& brightness) override;
    bool onAdjustBrightness(int& brightnessDelta) override;
    bool onColor(int& r, int& g, int& b) override;

    void updateStripe();

  protected:
    int pin_r;
    int pin_g;
    int pin_b;

    bool powerState;
    int  brightness;
    int  colorTemperature;
    
    byte r;
    byte g;
    byte b;
};

MyLight::MyLight(const String& deviceId, int pin_r, int pin_g, int pin_b)
    : SinricProLight(deviceId)
    , pin_red(pin_red)
    , pin_green(pin_green)
    , pin_blue(pin_blue) {
    pinMode(pin_r, OUTPUT);
    pinMode(pin_g, OUTPUT);
    pinMode(pin_b, OUTPUT);
}

bool MyLight::onPowerState(bool& powerState) {
    this->powerState = powerState;
    updateStripe();
    return true;
}

bool MyLight::onBrightness(int& brightness) {
    this->brightness = brightness;
    updateStripe();
    return true;
}

bool MyLight::onAdjustBrightness(int& brightnessDelta) {
    brightness += brightnessDelta;
    brightnessDelta = brightness;
    updateStripe();
    return true;
}

bool MyLight::onColor(byte& r, byte& g, byte& b) {
    this->r = r;
    this->g = g;
    this->b = b;
    updateStripe();
    return true;
}

void MyLight::updateStripe() {
    if (powerState) {
        analogWrite(pin_r, map(r * brightness, 0, 255 * 100, 0, 1023));
        analogWrite(pin_g, map(g * brightness, 0, 255 * 100, 0, 1023));
        analogWrite(pin_b, map(b * brightness, 0, 255 * 100, 0, 1023));
    } else {
        digitalWrite(pin_r, LOW);
        digitalWrite(pin_g, LOW);
        digitalWrite(pin_b, LOW);
    }
}