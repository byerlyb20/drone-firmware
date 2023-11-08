#include "flightcontrol.h"

#define ESC_PWM_FREQ 2000

FlightControl::FlightControl(int idle, int min, int max) {
    this->idle = idle;
    this->min = min;
    this->max = max;
}

void FlightControl::init(int a, int b, int c, int d) {
    frontLeft = a;
    frontRight = b;
    backLeft = c;
    backRight = d;
    pinMode(frontLeft, OUTPUT);
    pinMode(frontRight, OUTPUT);
    pinMode(backLeft, OUTPUT);
    pinMode(backRight, OUTPUT);
}

void FlightControl::cycle() {
    if (testing) {
        if (loopCount == 0) {
            setMotorSpeeds(TESTING_RUNNING_SPEED, TESTING_RUNNING_SPEED, TESTING_RUNNING_SPEED, TESTING_RUNNING_SPEED);
        }
        if (loopCount == 500) {
            setMotorSpeeds(TESTING_IDLE_SPEED, TESTING_IDLE_SPEED, TESTING_IDLE_SPEED, TESTING_IDLE_SPEED);
            loopCount = 0;
            testing = false;
        } else {
            loopCount++;
        }
    } else if (flying) {
        parseMoveValues();
    } else {
        setMotorSpeeds(0, 0, 0, 0);
    }
}

void FlightControl::move(float l, float r, float p, float y) {
    lift = l;
    roll = r;
    pitch = p;
    yaw = y;
}

void FlightControl::parseMoveValues() {
    // Input ranges:
    // Lift: 0 ... 1
    // Roll, Pitch, Yaw: -1 ... 1
    
    // Scale and weight lift, roll, pitch, and yaw
    roll /= 2;
    pitch /= 2;
    yaw /= 4;
    
    // Calculate motor outputs
    float m[4];
    m[0] = lift - pitch + yaw;
    m[1] = lift - roll - yaw;
    m[2] = lift + roll - yaw;
    m[3] = lift + pitch + yaw;
    for (int i = 0; i < 4; i++) {
        m[i] = constrain(m[i], 0.0, 1.0);
    }
    
    //Serial.println("Speed: " + String(m1));
    setMotorSpeeds(m[0], m[1], m[2], m[3]);
}

void FlightControl::test() {
    if (ready()) {
        testing = true;
    }
}

bool FlightControl::ready() {
    return !testing && !flying;
}

void FlightControl::setMotorSpeeds(float a, float b, float c, float d) {
    byte a_s = (byte) (a * 255);
    byte b_s = (byte) (b * 255);
    byte c_s = (byte) (c * 255);
    byte d_s = (byte) (d * 255);
    analogWrite(frontLeft, a_s, ESC_PWM_FREQ);
    analogWrite(frontRight, b_s, ESC_PWM_FREQ);
    analogWrite(backLeft, c_s, ESC_PWM_FREQ);
    analogWrite(backRight, d_s, ESC_PWM_FREQ);
}