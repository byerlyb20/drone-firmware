#ifndef BATTERY_H
#define BATTERY_H

#include "application.h"

class Battery {
    public:
        Battery();
        Battery(int pin);
        float getVoltage();
        float getChargePercent();
    private:
        int pin;
        float getRawPinVoltage();
};

#endif