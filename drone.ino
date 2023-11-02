// This #include statement was automatically added by the Particle IDE.
#include <vector>

#include "esc.h"
#include "flightcontrol.h"
#include "battery.h"
#include "indicators.h"

#define FD_SSID "FlightDeck-Comms"
#define FD_KEY ""

#define COMM_PORT 8080

IPAddress remote;
UDP connection;

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
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    control = FlightControl(1200, 1140, 1600);
    control.init(TX, RX, D2, D3);
    battery = Battery(A3);
    
    connection.begin(COMM_PORT);
    
    timer.start();
    
    Serial.println("Setup complete");
}

void loop() {
    // Data has been received
    if (connection.parsePacket() > 0) {
        if (!remote) {
            Serial.println("Handshake data received");
            
            // Handshake time! Store the IP Address for later use
            int event = connection.read();
            if (event == '5') {
                Serial.println("Handshake event confirmed");
                remote = connection.remoteIP();
                Serial.print("Remote IP Address: ");
                Serial.println(remote);

                char buffer[] = {1, 2};
                int sent = connection.sendPacket(buffer, sizeof(buffer), remote, COMM_PORT);
                    
                if (sent > 0) {
                    Serial.println("Handshake reply sent");
                } else {
                    Serial.println("Error occured when sending handshake reply");
                }
                delay(500);
            }
        } else if (connection.remoteIP() == remote) {
            //Serial.println("Normal data received");
            int event = connection.read();
            processEvent(event);
        }
        
        // Discard all remaining data in packet
        while(connection.available()) {
            connection.read();
        }
    }
    
    // Send telemetry data every 50 cycles
    if (remote) {
        if (loopsSinceTele == 50) {
            sendTelemetry();
            loopsSinceTele = 0;
        } else {
            loopsSinceTele++;
        }
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
        // Event 5 does exist, although it is not processed here
        // This code is a placeholder for reference purposes only
        case '5': {
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