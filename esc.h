#ifndef ESC_H
#define ESC_H

#include "application.h"

class ESC {
    public:
        ESC();
        ESC(int pwm);
        void cycle();
        void arm();
        void calibrate();
        bool ready();
        void setSpeed(int speed);
    private:
        int loopCount = 0;
        bool arming = false;
        bool calibrating = false;
        int pwmDuty = -1;
        int prevPwmDuty = 1000;
        Servo servo;
};

#endif