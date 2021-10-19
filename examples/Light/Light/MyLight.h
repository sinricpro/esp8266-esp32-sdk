#include <SinricProLight.h>

class MyLight : public SinricProLight {
    public:
      using SinricProLight::SinricProLight;

      bool onPowerState(bool& powerState) override;
      bool onBrightness(int& brightness) override;
      bool onAdjustBrightness(int& brightnessDelta) override;
      bool onColor(int& r, int& g, int& b) override;
      bool onColorTemperature(int& colorTemperature) override;
      bool onIncreaseColorTemperature(int& colorTemperature) override;
      bool onDecreaseColorTemperature(int& colorTemperature) override;

    protected:
      bool powerState;
      int brightness;
      int colorTemperature;
      byte r;
      byte g;
      byte b;
};

bool MyLight::onPowerState(bool& powerState) {
    this->powerState = powerState;
    Serial.printf("Light turned %s\r\n", powerState ? "on" : "off");
    return true;
}

bool MyLight::onBrightness(int& brightness) {
    this->brightness = brightness;
    Serial.printf("brightness changed to %d\r\n", brightness);
    return true;
}

bool MyLight::onAdjustBrightness(int& brightnessDelta) {
    brightness += brightnessDelta;
    Serial.printf("brightnes changed about %d to %d\r\n", brightnessDelta, brightness);
    brightnessDelta = brightness;
    return true;
}

bool MyLight::onColor(byte& r, byte& g, byte& b) {
    this->r = r;
    this->g = g;
    this->b = b;
    Serial.printf("Color set to r: %d, g: %d, b: %d\r\n", r, g, b);
    return true;
}

bool MyLight::onColorTemperature(int& colorTemperature) {
    Serial.printf("Colortemperature set to %d\r\n", colorTemperature);
    this->colorTemperature = colorTemperature;
}

bool MyLight::onIncreaseColorTemperature(int& colorTemperature) {
    this->colorTemperature++;
    Serial.printf("ColorTemperature increased to %d\r\n", this->colorTemperature);
    colorTemperature = this->colorTemperature;
    return true;
}

bool MyLight::onDecreaseColorTemperature(int& colorTemperature) {
    this->colorTemperature--;
    Serial.printf("ColorTemperature decreased to %d\r\n", this->colorTemperature);
    colorTemperature = this->colorTemperature;
    return true;
}