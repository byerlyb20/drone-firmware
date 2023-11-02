#ifndef FLIGHTCONTROL_H
#define FLIGHTCONTROL_H

#include "math.h"
#include "esc.h"

class FlightControl {
    public:
        FlightControl();
        FlightControl(int idle, int min, int max);
        void cycle();
        void init(int a, int b, int c, int d);
        void takeoff();
        void endTakeoff();
        void move(float lift, float roll, float pitch, float yaw);
        void test();
        bool ready();
    private:
        ESC frontLeft,frontRight, backLeft, backRight;
        int idle, min, max;
        int loopCount = 0;
        bool testing = false;
        bool takeoffing = false;
        bool flying = false;
        float lift, roll, pitch, yaw;
        void parseMoveValues();
};

#endif