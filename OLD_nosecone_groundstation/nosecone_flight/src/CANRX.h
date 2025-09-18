#ifndef CANRX_H
#define CANRX_H

#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#include "HAL.h"

namespace CANRX {
    const long MAX_SIZE = 256;
    const uint8_t CANRX = HAL::CANRX;
    const uint8_t CANTX = HAL::CANTX;
    const uint32_t id = 0x7DF;

    CanFrame rxFrame;
    char msgBuf[MAX_SIZE];
    int dataInd = -1;

    // 0 == message hasn't started, 1 == message started, 2 = msgReady
    int msgDecodeState = 0;
    String decoded_msg = "";

    void printFrameData(CanFrame f) {
        Serial.print("----------------------------------------\n|");
        for (int i = 0; i < 8; i++) {
            uint8_t val = f.data[i];
            if (val < 32 || val > 126) {
                Serial.print("(" + String(val) + ")");
            } else {
                Serial.print(char(val));
            }
        }
        Serial.println("|\n----------------------------------------\n");
    }

    // does whatever it needs to do with complete CAN message
    void handleMessage(String str) {
        // Serial.println("received string = " + str);
        // Serial.println("handling msg");
        decoded_msg = str;
        msgDecodeState = 2;
    }

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
    }

    void decodeMessage(uint32_t blocktime) {
        if(ESP32Can.readFrame(rxFrame, blocktime)) {   // previously 1000ms, set to 0 for non blocking
            // Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
            // printFrameData(rxFrame);
            // return if identifier invalid or if current message hasn't been consumed (==2)
            if (msgDecodeState == 2 || rxFrame.identifier != id) {return;}

            for (int j = 0; j < 8; j++) {
                // given frame is start of string
                if (j == 0 && rxFrame.data[0] == 0xFE) {
                    dataInd = 0;
                    // set state to "actively decoding"/"decoding started"
                    msgDecodeState = 1;
                    continue;
                }
                // ignore frame for invalid index
                if (dataInd < 0) {return;}

                // reached end or max buffer size
                if (dataInd >= MAX_SIZE || rxFrame.data[j] == 0xFF) {
                    int index = (dataInd >= MAX_SIZE) ? (MAX_SIZE - 1) : dataInd;
                    // copy null char
                    msgBuf[index] = '\0';
                    // get complete message
                    handleMessage(String(msgBuf));
                    dataInd = -1;
                    return;
                }

                // normal case: middle of message
                msgBuf[dataInd] = rxFrame.data[j];
                dataInd++;
            }
        }
        return;
    }


}

#endif