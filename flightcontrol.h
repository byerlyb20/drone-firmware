#ifndef FLIGHTCONTROL_H
#define FLIGHTCONTROL_H

#include "math.h"

class FlightControl {
    public:
        FlightControl(int idle, int min, int max);
        void cycle();
        void init(int a, int b, int c, int d);
        void move(float lift, float roll, float pitch, float yaw);
        void test();
        bool ready();
    private:
        const float TESTING_IDLE_SPEED = 0.25;
        const float TESTING_RUNNING_SPEED = 0.35;
        int frontLeft,frontRight, backLeft, backRight;
        int idle, min, max;
        int loopCount = 0;
        bool testing = false;
        bool flying = false;
        float lift, roll, pitch, yaw;
        void parseMoveValues();
        void setMotorSpeeds(float a, float b, float c, float d);
};

#endif