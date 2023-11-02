#ifndef INDICATORS_H
#define INDICATORS_H

#include "application.h"

#define INDICATOR_MODE_BLINK 0
#define INDICATOR_MODE_ACTIVE 1

class Indicators {
    public:
        Indicators();
        void cycle();
        void init(int a, int b);
        void setStyle(int mode, int speed);
    private:
        int a, b, c, d;
        int loopCount = 0;
        int speed = 1000;
        int indicatorMode = INDICATOR_MODE_BLINK;
};

#endif