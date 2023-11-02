#include "flightcontrol.h"

#define TESTING_IDLE_SPEED 1000
#define TESTING_RUNNING_SPEED 1140
#define TAKEOFF_IDLE_SPEED 1140

FlightControl::FlightControl() {}

FlightControl::FlightControl(int idle, int min, int max) {
    this->idle = idle;
    this->min = min;
    this->max = max;
}

void FlightControl::init(int a, int b, int c, int d) {
    frontLeft = ESC(a);
    frontRight = ESC(b);
    backLeft = ESC(c);
    backRight = ESC(d);
    frontLeft.arm();
    frontRight.arm();
    backLeft.arm();
    backRight.arm();
}

void FlightControl::cycle() {
    if (testing) {
        if (loopCount == 0) {
            frontLeft.setSpeed(TESTING_RUNNING_SPEED);
            frontRight.setSpeed(TESTING_RUNNING_SPEED);
            backLeft.setSpeed(TESTING_RUNNING_SPEED);
            backRight.setSpeed(TESTING_RUNNING_SPEED);
            
        }
        if (loopCount == 500) {
            frontLeft.setSpeed(TESTING_IDLE_SPEED);
            frontRight.setSpeed(TESTING_IDLE_SPEED);
            backLeft.setSpeed(TESTING_IDLE_SPEED);
            backRight.setSpeed(TESTING_IDLE_SPEED);
            loopCount = 0;
            testing = false;
        } else {
            loopCount++;
        }
    } else if (takeoffing) {
        // Only take positive values
        float liftm = (lift > 0 ? lift : 0);
        
        int speed = liftm * (2000 - TAKEOFF_IDLE_SPEED) + TAKEOFF_IDLE_SPEED;
        Serial.println("Speed: " + String(speed));
        
        frontLeft.setSpeed(speed);
        frontRight.setSpeed(speed);
        backLeft.setSpeed(speed);
        backRight.setSpeed(speed);
        if (speed >= 1300) {
            takeoffing = false;
            flying = true;
        }
    } else if (flying) {
        parseMoveValues();
    } else {
        frontLeft.setSpeed(1000);
        frontRight.setSpeed(1000);
        backLeft.setSpeed(1000);
        backRight.setSpeed(1000);
    }
    
    frontLeft.cycle();
    frontRight.cycle();
    backLeft.cycle();
    backRight.cycle();
}

void FlightControl::move(float l, float r, float p, float y) {
    lift = l;
    roll = r;
    pitch = p;
    yaw = y;
}

void FlightControl::parseMoveValues() {
    // Initialize boundaries for motor output
    int upperEnd = max - idle;
    int lowerEnd = idle - min;

    // Scale and weight lift, roll, pitch, and yaw
    float total = fabs(lift) + fabs(roll) + fabs(pitch) + fabs(yaw);
    lift = fabs(lift) / total * lift;
    roll = fabs(roll) / total * roll;
    pitch = fabs(pitch) / total * pitch;
    yaw = fabs(yaw) / total * yaw;

    // Calculate motor outputs
    float m1 = lift + roll - pitch + yaw;
    float m2 = lift - roll - pitch - yaw;
    float m3 = lift + roll + pitch - yaw;
    float m4 = lift - roll + pitch + yaw;

    // Scale motor outputs
    m1 = m1 * (m1 > 0 ? upperEnd : lowerEnd) + idle;
    m2 = m2 * (m2 > 0 ? upperEnd : lowerEnd) + idle;
    m3 = m3 * (m3 > 0 ? upperEnd : lowerEnd) + idle;
    m4 = m4 * (m4 > 0 ? upperEnd : lowerEnd) + idle;
    
    //Serial.println("Speed: " + String(m1));
    frontLeft.setSpeed(m1);
    frontRight.setSpeed(m2);
    backLeft.setSpeed(m3);
    backRight.setSpeed(m4);
}

void FlightControl::test() {
    if (ready()) {
        testing = true;
    }
}

void FlightControl::takeoff() {
    if (ready()) {
        flying = true;
    }
}

void FlightControl::endTakeoff() {
    takeoffing = false;
    flying = false;
}

bool FlightControl::ready() {
    return !testing && !takeoffing && !flying;
}