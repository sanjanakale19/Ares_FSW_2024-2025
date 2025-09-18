#ifndef ADS8688_PT_H
#define ADS8688_PT_H

#include <Arduino.h>
#include "../lib/ADS8688/ADS8688_LIB.h"
#include "HAL.h"

// Chamber PT calibration from a month ago: a=0.2521919592 b=-238.3279503
// Press Tank: m=2.029440568, c=3.048591747
// Fuel Tank: m=0.2526123106, c=-242.9702837
// Ox Tank: m=0.2523605766, c=-237.3301832

// channel 0
float ox_m = 0.2523605766;
float ox_b = -237.3301832;

// channel 1
float press_m = 2.029440568;
float press_b = 3.048591747;

// channel 2
float fuel_m = 0.2526123106;
float fuel_b = -242.9702837;

// channel 3
float cc_m = 0.2521919592;
float cc_b = -238.3279503;

namespace ADS8688_PT {

    ADS8688 adc; // Create ADC instance
    int lastRead = 0;

    // Calibration Coefficients (y=ax+b)
    // 0 = ox, 1 = press, 2 = fuel, 3 = CC
    const float a[4] = {ox_m, press_m, fuel_m, cc_m}; // multipliers
    const float b[4] = {ox_b, press_b, fuel_b, cc_b}; // offsets

    // Define custom SPI pins for ADS1256 Module
    const uint8_t MISO = HAL::HMISO_PIN;
    const uint8_t SCLK = HAL::HSCK_PIN;
    const uint8_t MOSI = HAL::HMOSI_PIN;
    const uint8_t CS = HAL::ADS8688_CS;

    float pt0, pt1, pt2, pt3;

    float ox_pt, press_pt, fuel_pt, cc_pt;

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

            // calibrate PTs according to calibration curves
            ox_pt = a[0] * pt0 + b[0];
            press_pt = a[1] * pt1 + b[1];
            fuel_pt = a[2] * pt2 + b[2];
            cc_pt = a[3] * pt3 + b[3];

            delay(20);
        }
    }
}

#endif