#include "battery.h"

Battery::Battery() {}

// First parameter is the pin connected to the voltage divider on the breadboard (steps down the voltage from the Vcc terminal)
Battery::Battery(int pin) {
    this->pin = pin;
}

float Battery::getVoltage() {
    return getRawPinVoltage() / (220.0f / 1220.0f);
}

float Battery::getChargePercent() {
    return 0.0;
}

// From the docs: analogRead() will "map input voltages between 0 and 3.3 volts into integer values between 0 and 4095"
float Battery::getRawPinVoltage() {
    return (analogRead(pin) / 4095.0f) * 3.3f;
}