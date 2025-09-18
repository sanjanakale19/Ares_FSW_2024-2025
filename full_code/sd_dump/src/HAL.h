#ifndef HAL_H
#define HAL_H

#include <Arduino.h>
#include "Globals.h"
#include <HardwareSerial.h>
#include <SPI.h>
#include <Wire.h>

namespace HAL { 

    // HSPI
    const uint8_t HSCK_PIN = 17;
    const uint8_t HMISO_PIN = 16;
    const uint8_t HMOSI_PIN = 15;
    SPIClass* HSPI_bus;

    // SD/Flash
    const uint8_t SD_CS = 14;

    void initSensorHAL() {
        HSPI_bus = new SPIClass(HSPI);
        HSPI_bus->begin(HSCK_PIN, HMISO_PIN, HMOSI_PIN);
    }   
}

#endif