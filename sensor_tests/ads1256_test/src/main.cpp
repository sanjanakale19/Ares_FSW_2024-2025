#include <Arduino.h>
#include <ADS1256.h>

// Define custom SPI pins for ADS1256 Module
#define SPI_MISO 11
#define SPI_SCLK 14
#define SPI_MOSI 13
#define SPI_CS  4

#define DRDY 15

// Define LED indicator pin
// #define LED 7

SPIClass spi_ads(FSPI); // Create custom SPI instance
ADS1256 ADC(&spi_ads, DRDY, SPI_CS, 2.5, -1, -1); // Instantiate ADS1256

// Calibration coefficients y=Ax+B
float calibrationA = -4.54 / 1000.0;
float calibrationB = 0.083;

// convertToWeight function
// Returns measured force (lbf)
float convertToWeight(float voltage) {
  return (calibrationA * voltage) + calibrationB;
}

void setup()
{
    // Initialize Serial
    Serial.begin(115200);
    delay(100);

    // Configure SPI pins
    pinMode(SPI_MISO, INPUT_PULLUP);
    pinMode(SPI_SCLK, OUTPUT);
    pinMode(SPI_MOSI, OUTPUT);

    // Initialize the SPI bus with custom pins
    spi_ads.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS);

    // ADC Setup
    ADC.InitializeADC(); 

    // Set Gain (PGA)
    ADC.setPGA(PGA_64);  // Maximum gain for small signals

    // Set Differential Input Channels (AIN0 - AIN1)
    ADC.setMUX(DIFF_0_1);  

    // Set Sampling Rate
    ADC.setDRATE(DRATE_1000SPS);  

    delay(100);
}

void loop()
{
  // Read differential voltage
  long rawConversion = ADC.readSingleContinuous();    

  // Convert raw ADC value to voltage
  float voltageValue = ADC.convertToVoltage(rawConversion) * 1000; 

  // Convert voltage to weight
  // float weight = convertToWeight(voltageValue);      

  // Print results
  Serial.print(String(-voltageValue, 5));
  Serial.println();
}