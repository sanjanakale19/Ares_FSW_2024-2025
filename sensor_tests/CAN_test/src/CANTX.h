#ifndef CANTX_H
#define CANTX_H

#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

namespace CANTX {
    const uint8_t CANRX = 10;
    const uint8_t CANTX = 9;
    const uint32_t identifier = 0x7DF;

    CanFrame frame = {0};

    uint8_t sample[8] = {254, 104, 101, 108, 108, 255, 170, 170};

    // replaces all bytes in data buffer with 0xAA (defined as null character for TWAI)
    void clearFrameData() {
        for (int i = 0; i < 8; i++) {
            frame.data[i] = 0xAA;
        }
    }

    void printFrameData() {
        Serial.print("----------------------------------------\n|");
        for (int i = 0; i < 8; i++) {
            uint8_t val = frame.data[i];
            if (val < 32 || val > 126) {
                Serial.print("(" + String(val) + ")");
            } else {
                Serial.print(char(val));
            }
        }
        Serial.println("|\n----------------------------------------\n");
        delay(20);
    }

    // initialize CAN bus with all default parameters
    void setupCAN() {
        // Set pins
        ESP32Can.setPins(CANTX, CANRX);

        // You can set custom size for the queues - those are default
        ESP32Can.setRxQueueSize(5);
        ESP32Can.setTxQueueSize(5);
        
        // or override everything in one command;
        // It is also safe to use .begin() without .end() as it calls it internally
        if(ESP32Can.begin(ESP32Can.convertSpeed(500), CANTX, CANRX, 10, 10)) {
            Serial.println("CAN bus started!");
        } else {
            Serial.println("CAN bus failed!");
        }

        frame.identifier = 0x7DF;
        frame.extd = 0;
        frame.data_length_code = 8;
        clearFrameData();
    }

    void encodeMessage(String s) {
        // ignore degenerate string
        if (s.length() < 1) return;

        const char* datastr = s.c_str();
        int dataInd = 0;

        while (true) {
            clearFrameData();
            for (int j = 0; j < 8; j++) {
                // beginning of string
                if (dataInd == 0) {
                    frame.data[0] = 0xFE;
                    frame.data[1] = datastr[0];
                    j = 1;
                } 
                // end of string
                else if (datastr[dataInd] == '\0') {
                    frame.data[j] = 0xFF;

                    // done writing all bytes, transmit and exit
                    ESP32Can.writeFrame(frame);
                    delay(10);
                    // printFrameData();
                    return;
                }
                // all other cases
                else {
                    frame.data[j] = datastr[dataInd];
                }
                dataInd++;
            }
            // current frame full, transmit data
            ESP32Can.writeFrame(frame);
            delay(10);
            // printFrameData();
        }

    }

}

#endif