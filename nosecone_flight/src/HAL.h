#ifndef HAL_H
#define HAL_H

#include <Arduino.h>
#include "Globals.h"
#include <HardwareSerial.h>
#include <SPI.h>
#include <Wire.h>

namespace HAL { 

    // VSPI
    const uint8_t VSCK_PIN = 18;
    const uint8_t VMISO_PIN = 35;
    const uint8_t VMOSI_PIN = 23;
    SPIClass* VSPI_bus;

    // HSPI
    const uint8_t HSCK_PIN = 14;
    const uint8_t HMISO_PIN = 34;
    const uint8_t HMOSI_PIN = 13;
    SPIClass* HSPI_bus;

    // I2C
    const uint8_t INA_ADDR = 0x40;
    const uint8_t INA_SDA_PIN = 5;
    const uint8_t INA_SCL_PIN = 19;

    // Radio
    const uint8_t TRANSMITTER_CS = 17;
    const uint8_t RADIO_TXEN = 21;
    const uint8_t RADIO_RXEN = 22;

    // Altimeters
    const uint8_t ALT1_BMP390_CS = 32;
    const uint8_t ALT2_MS5607_CS = 15;

    // IMUs
    const uint8_t ICM20948_CS = 33;
    const uint8_t BMI323_CS = 16;

    // SD/Flash
    const uint8_t SD_CS = 4;

    // CAN TX/RX
    const uint8_t CANRX = 26;
    const uint8_t CANTX = 25;

    void initCSPins() {
        pinMode(TRANSMITTER_CS, OUTPUT);
        pinMode(ALT1_BMP390_CS, OUTPUT);
        pinMode(ALT2_MS5607_CS, OUTPUT);
        pinMode(ICM20948_CS, OUTPUT);
        pinMode(BMI323_CS, OUTPUT);
        pinMode(SD_CS, OUTPUT);
        
        digitalWrite(TRANSMITTER_CS, HIGH);
        digitalWrite(ALT1_BMP390_CS, HIGH);
        digitalWrite(ALT2_MS5607_CS, HIGH);
        digitalWrite(ICM20948_CS, HIGH);
        digitalWrite(BMI323_CS, HIGH);
        digitalWrite(SD_CS, HIGH);
    }

    void initHSPI_HAL() {
        HSPI_bus = new SPIClass(HSPI);
        HSPI_bus->begin(HSCK_PIN, HMISO_PIN, HMOSI_PIN);
    }   

    void initVSPI_HAL() {
        VSPI_bus = new SPIClass(VSPI);
        VSPI_bus->begin(VSCK_PIN, VMISO_PIN, VMOSI_PIN);
    } 
}

#endif