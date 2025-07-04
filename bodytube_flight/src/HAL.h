#ifndef HAL_H
#define HAL_H

#include <Arduino.h>
#include "Globals.h"

#include <HardwareSerial.h>
#include <SPI.h>

namespace HAL { 

    // VSPI
    const uint8_t VSCK_PIN = 14;
    const uint8_t VMISO_PIN = 11;
    const uint8_t VMOSI_PIN = 13;
    SPIClass* VSPI_bus;

    // HSPI
    const uint8_t HSCK_PIN = 18;
    const uint8_t HMISO_PIN = 17;
    const uint8_t HMOSI_PIN = 8;
    SPIClass* HSPI_bus;

    // I2C
    const uint8_t INA_SDA_PIN = 5;
    const uint8_t INA_SCL_PIN = 6;

    // Altimeter
    const uint8_t MS5607_CS = 21;

    // IMU
    const uint8_t BMI323_CS = 37;

    // Shock Accelerometer
    const uint8_t Shock_CS = 7;
    const uint8_t Shock_INT_CS = 3;

    // ADS1256
    const uint8_t ADS1256_CS = 4;
    const uint8_t ADS_DRDY = 15;

    // ADS8688
    const uint8_t ADS8688_CS = 38;
    
    // SD/Flash
    const uint8_t SD_CS = 36;

    // CAN TX/RX
    const uint8_t CANTX = 9;
    const uint8_t CANRX = 10;

    // Digital Signal
    const uint8_t IO12 = 12;

    // Indication LED
    const uint8_t TEST_LED = 35;

    void initCSPins() {
        // write all cs pins high
        pinMode(MS5607_CS, OUTPUT);
        pinMode(BMI323_CS, OUTPUT);
        pinMode(Shock_CS, OUTPUT);
        pinMode(ADS1256_CS, OUTPUT);
        pinMode(ADS_DRDY, INPUT);
        pinMode(ADS8688_CS, OUTPUT);
        pinMode(SD_CS, OUTPUT);
        pinMode(IO12, OUTPUT);
        pinMode(TEST_LED, OUTPUT);

        digitalWrite(MS5607_CS, HIGH);
        digitalWrite(BMI323_CS, HIGH);
        digitalWrite(Shock_CS, HIGH);
        digitalWrite(ADS1256_CS, HIGH);
        digitalWrite(ADS_DRDY, HIGH);
        digitalWrite(ADS8688_CS, HIGH);
        digitalWrite(SD_CS, HIGH);

        // IO12 write high
        digitalWrite(IO12, HIGH);

        // TEST LED write high
        digitalWrite(TEST_LED, HIGH);
    }

    void initHSPI() {
        HSPI_bus = new SPIClass(HSPI);
        HSPI_bus->begin(HSCK_PIN, HMISO_PIN, HMOSI_PIN);
    }   

    void initVSPI() {
        VSPI_bus = new SPIClass(FSPI);
        VSPI_bus->begin(VSCK_PIN, VMISO_PIN, VMOSI_PIN);
    } 
}


#endif