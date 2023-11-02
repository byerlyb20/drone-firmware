#include "indicators.h"

Indicators::Indicators() {}

void Indicators::init(int a, int b) {
    this->a = a;
    this->b = b;
    
    pinMode(a, OUTPUT);
    pinMode(b, OUTPUT);
}

void Indicators::setStyle(int mode, int speed) {
    if (indicatorMode != mode || this->speed != speed) {
        indicatorMode = mode;
        this->speed = speed;
        loopCount = 0;
    }
}

void Indicators::cycle() {
    switch (indicatorMode) {
        case INDICATOR_MODE_BLINK:
            if (loopCount == 0) {
                digitalWrite(a, LOW);
                digitalWrite(b, HIGH);
            } else if (loopCount == speed) {
                digitalWrite(a, HIGH);
                digitalWrite(b, LOW);
            }
            if (loopCount == (speed * 2)) {
                loopCount = 0;
            } else {
                loopCount++;
            }
            break;
        case INDICATOR_MODE_ACTIVE:
            if (loopCount == 0) {
                digitalWrite(a, LOW);
                digitalWrite(b, HIGH);
            } else if (loopCount == speed) {
                digitalWrite(a, HIGH);
                digitalWrite(b, LOW);
            } else if (loopCount == (speed * 2)) {
                digitalWrite(a, LOW);
                digitalWrite(b, LOW);
            }
            if (loopCount == (speed * 15)) {
                loopCount = 0;
            } else {
                loopCount++;
            }
            break;
    }
}