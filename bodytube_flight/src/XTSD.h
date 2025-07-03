#ifndef XTSD_H
#define XTSD_H

#include "HAL.h"
#include <SD.h>


namespace XTSD {
    
    File dataFile;
    const char* fileName = "/test.txt";

    String logStr = "";

    int logTime = 0;
    int someTime = 0;
    int oldLog = 0;
    long lastLog = 0;
    bool logSuccess = false;

    void setupSD() {
        if(!SD.begin(HAL::SD_CS, *HAL::HSPI_bus)) {
            Serial.println("initialization failed!");
            return;
        } 

        // overwrites every time
        dataFile = SD.open(fileName, FILE_WRITE);

        // TODO: BEFORE LAUNCH CHANGE BACK TO APPEND
        // dataFile = SD.open(fileName, FILE_APPEND);
    }

    void logSD(String str) {
        
        // someTime = millis();
        // dataFile = SD.open(fileName, FILE_APPEND);
        
        if (millis() - lastLog > 5000) {
            dataFile.close();
            dataFile = SD.open(fileName, FILE_APPEND);
            lastLog = millis();
            DEBUGLN("Saved");
        }

        if (dataFile) {
            
            DEBUG("Writing to ");
            DEBUG(fileName);
            DEBUGLN("...");
            
            dataFile.println(str);

            logSuccess = true;

            // dataFile.close();
            
            DEBUGLN("done.");
        }
        else {
            DEBUG("Error writing to ");
            DEBUGLN(fileName);

            logSuccess = false;
        }
        // someTime = millis() - someTime;
        // Serial.println(someTime);
    }   

}


#endif