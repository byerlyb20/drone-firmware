#include "esc.h"

#define MAXIMUM_MOTOR_ACCEL 5

ESC::ESC() {}

ESC::ESC(int pwm) {
    servo.attach(pwm);
}

void ESC::cycle() {
    if (arming) {
        if (loopCount == 0) {
            servo.writeMicroseconds(1000);
        }
        if (loopCount == 250) {
            loopCount = 0;
            arming = false;
        } else {
            loopCount++;
        }
    } else if (calibrating) {
        if (loopCount == 0) {
            servo.writeMicroseconds(2000);
        } else if (loopCount == 500) {
            servo.writeMicroseconds(1000);
        }
        if (loopCount == 1000) {
            loopCount = 0;
            calibrating = false;
        } else {
            loopCount++;
        }
    } else if (pwmDuty >= 0) {
        int diff = pwmDuty - prevPwmDuty;
        if (abs(diff) > MAXIMUM_MOTOR_ACCEL) {
            // The requested pwm duty change is too high, level it off
            int finalPwmDuty = prevPwmDuty + (diff > 0 ? MAXIMUM_MOTOR_ACCEL : -MAXIMUM_MOTOR_ACCEL);
            servo.writeMicroseconds(finalPwmDuty);
            prevPwmDuty = finalPwmDuty;
        } else {
            // The requested pwm duty change is within a reasonable range
            servo.writeMicroseconds(pwmDuty);
            prevPwmDuty = pwmDuty;
        }
    }
}

void ESC::arm() {
    if (ready()) {
        arming = true;
    }
}

void ESC::calibrate() {
    if (ready()) {
        calibrating = true;
    }
}

void ESC::setSpeed(int speed) {
    pwmDuty = speed;
}

bool ESC::ready() {
    return !calibrating && !arming;
}