// This #include statement was automatically added by the Particle IDE.
#include <vector>

#include "esc.h"
#include "flightcontrol.h"
#include "battery.h"
#include "indicators.h"

#define FD_SSID "FlightDeck-Comms"
#define FD_KEY ""

#define COMM_PORT 8080

bool remoteLatch = false;
IPAddress remote;
UDP connection;
bool wifiConnected = false;

FlightControl control;
Battery battery;

int loopsSinceTele = 0;
Timer timer(20, cycle);

//SYSTEM_MODE(MANUAL);

void setup() {
    // Setup WiFi
    /*WiFi.setHostname("FlightDeck-Target");
    if (!checkAPExists()) {
        WiFi.setCredentials(FD_SSID, FD_KEY, WPA2, WLAN_CIPHER_AES);
    }
    WiFi.on();
    WiFi.connect();
    waitUntil(WiFi.ready);*/
    
    Serial.begin(9600);
    
    control = FlightControl(1200, 1140, 1600);
    control.init(TX, RX, D2, D3);
    battery = Battery(A3);
    
    timer.start();
    
    Serial.println("Setup complete");
}

void loop() {
    if (WiFi.ready()) {
        // Init UDP server every time WiFi comes online
        if (!wifiConnected) {
            connection.begin(COMM_PORT);
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            wifiConnected = true;
        }

        // Data has been received
        if (connection.parsePacket() > 0) {
            int event = connection.read();
            if (!remoteLatch) {
                // Handshake: latch the remote by storing it's IP address and
                // checking future incoming packets against it
                if (event == '5') {
                    remote = connection.remoteIP();
                    Serial.print("Remote IP Address: ");
                    Serial.println(remote);
                    remoteLatch = true;
                }
            }
            if (remoteLatch && connection.remoteIP() == remote) {
                //Serial.println("Normal data received");
                processEvent(event);
            }
            
            // Discard all remaining data in packet
            while (connection.available()) {
                connection.read();
            }
        }

        // Send telemetry data every 50 cycles after remote latch
        if (remoteLatch) {
            if (loopsSinceTele == 50) {
                sendTelemetry();
                loopsSinceTele = 0;
            } else {
                loopsSinceTele++;
            }
        }
    } else {
        wifiConnected = false;
    }
}

void cycle() {
    control.cycle();
}

void processEvent(int event) {
    switch (event) {
        case '0': {
            float lift = byteToFloat(connection.read(), connection.read(), connection.read(), connection.read());
            float roll = byteToFloat(connection.read(), connection.read(), connection.read(), connection.read());
            float pitch = byteToFloat(connection.read(), connection.read(), connection.read(), connection.read());
            float yaw = byteToFloat(connection.read(), connection.read(), connection.read(), connection.read());
            //Serial.println(String(lift) + "    " + String(roll) + "    " + String(pitch) + "    " + String(yaw));
            control.move(lift, roll, pitch, yaw);
            break;
        }
        case '1': {
            Serial.println("Initiating Test Sequence");
            control.test();
            break;
        }
        case '2': {
            Serial.println("Beginning Takeoff");
            control.takeoff();
            break;
        }
        case '4': {
            Serial.println("End Takeoff");
            control.endTakeoff();
            break;
        }
        case '5': {
            char buffer[] = {1, 2};
            connection.sendPacket(buffer, sizeof(buffer), remote, COMM_PORT);
            break;
        }
    }
}

bool checkAPExists() {
    WiFiAccessPoint ap[5];
    int found = WiFi.getCredentials(ap, 5);
    for (int i = 0; i < found; i++) {
        if (ap[i].ssid == FD_SSID) {
            return true;
        }
    }
    return false;
}

void sendTelemetry() {
    connection.beginPacket(remote, COMM_PORT);
    
    float voltage = battery.getVoltage();
    //float voltage = 10.0f;
    byte p[sizeof voltage];
    memcpy(p, &voltage, sizeof voltage);
    //client.write(p, 4);
    connection.write(p[0]);
    connection.write(p[1]);
    connection.write(p[2]);
    connection.write(p[3]);
    
    int ss = WiFi.RSSI();
    //int ss = -60;
    byte q[sizeof ss];
    memcpy(q, &ss, sizeof ss);
    //client.write(q, 4);
    connection.write(q[0]);
    connection.write(q[1]);
    connection.write(q[2]);
    connection.write(q[3]);
    
    connection.endPacket();
}

float byteToFloat(byte a, byte b, byte c, byte d) {
    byte array[4] = {d, c, b, a};
    float f = *(float *)&array;
    return f;
}