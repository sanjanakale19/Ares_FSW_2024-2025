#ifndef ADS1256_LOAD_H
#define ADS1256_LOAD_H

#include <Arduino.h>
#include <../lib/ADS1256/ADS1256_LIB.h>
#include "HAL.h"

namespace ADS1256_LOAD {

  ADS1256* ADC; // Declare ADS1256 pointer

  float voltageValue;
  float weight;

  // Calibration coefficients y=Ax+B
  float calibrationA = -4.54 / 1000.0;
  float calibrationB = 0.083;

  // convertToWeight function
  // Returns measured force (lbf)
  float convertToWeight(float voltage) {
    return (calibrationA * voltage) + calibrationB;
  }

  void setupADS() {
    // Instantiate ADS1256
    ADC = new ADS1256(HAL::VSPI_bus, HAL::ADS_DRDY, HAL::ADS1256_CS, 2.5, -1, -1);

    // ADC Setup
    ADC->InitializeADC(); 

    // Set Gain (PGA)
    ADC->setPGA(PGA_64);  // Maximum gain for small signals

    // Set Differential Input Channels (AIN0 - AIN1)
    ADC->setMUX(DIFF_0_1);  

    // Set Sampling Rate
    ADC->setDRATE(DRATE_1000SPS);  

    delay(100);
  }

  void readADS() {
    // Read differential voltage
    long rawConversion = ADC->readSingleContinuous();    

    // Convert raw ADC value to voltage
    voltageValue = ADC->convertToVoltage(rawConversion) * 1000; 

    // Convert voltage to weight
    weight = convertToWeight(voltageValue);      

    // Print results
    // Serial.println(String(weight, 10));
  }
}

#endif