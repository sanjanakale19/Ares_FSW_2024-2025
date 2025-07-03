#ifndef ADS8688_PT_H
#define ADS8688_PT_H

#include <Arduino.h>
#include "../lib/ADS8688/ADS8688_LIB.h"
#include "HAL.h"

namespace ADS8688_PT {

    ADS8688 adc; // Create ADC instance
    int lastRead = 0;

    // Calibration Coefficients (y=ax+b)
    const float a[8] = {1, 1, 1, 1, 1, 1, 1, 1}; // multipliers
    const float b[8] = {0, 0, 0, 0, 0, 0, 0 ,0}; // offsets

    // Define custom SPI pins for ADS1256 Module
    const uint8_t MISO = HAL::HMISO_PIN;
    const uint8_t SCLK = HAL::HSCK_PIN;
    const uint8_t MOSI = HAL::HMOSI_PIN;
    const uint8_t CS = HAL::ADS8688_CS;

    float pt0, pt1, pt2, pt3;

    void setupADS8688() {
        // Initialize ADCs
        adc.begin(HAL::HSPI_bus, CS, 4.1, 0x05);
        adc.setInputRange(CS, 0x05);
    }

    void readADS(long millis) {
        if (millis - lastRead > 47) {
            lastRead = millis;
            float voltages[8]; // Contains 8 Pressure Readings
            float calibratedVoltages[8] = {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff}; // Store calibrated values

            // Read PTs from ADC (CS)
            adc.readAllChannels(CS, true, voltages);

            // Apply calibration to PTs
            for (int i = 0; i < 8; i++) {
                calibratedVoltages[i] = 0.5f * voltages[i];
            }

            // unclear why it maps to channels 2-5 when hardware should be set to 0-4 but don't question it
            pt0 = calibratedVoltages[2];
            pt1 = calibratedVoltages[3];
            pt2 = calibratedVoltages[4];
            pt3 = calibratedVoltages[5];

            // for (int i = 0; i < 7; i++) {
            //     DEBUGLN(String(calibratedVoltages[i],2) + ",");
            // }
            // DEBUGLN(String(calibratedVoltages[7],2)); // Last value without trailing comma */
            delay(20);
        }
    }
}

#endif