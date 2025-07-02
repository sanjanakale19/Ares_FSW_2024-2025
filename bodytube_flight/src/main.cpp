#include "Globals.h"
#include "HAL.h"
#include "MS.h"
#include "XTSD.h"
#include "INA.h"
#include "ADS8688.h"
#include "ADS1256.h"
#include <Arduino.h>

// getting SD packet to log SD card
String getSDstr() {
  String s = (String)micros() + ",";
  s += String(XTSD::logTime) + ",";
  // altimeters
  s += String(MS::temp, 2) + "," + String(MS::pressure, 2) + "," + String(MS::altitude, 2) + ",";
  // ADS8688 PT readings
  s += String(ADS8688_PT::pt0, 3) + "," + String(ADS8688_PT::pt1, 3) + ",";
  s += String(ADS8688_PT::pt2, 3) + "," + String(ADS8688_PT::pt3, 3) + ",";
  // ADS1256 load cell readings
  s += String(ADS1256_LOAD::weight, 8) + ",";
  // INA
  s += String(INA::bus_voltage, 3) + ",";
  // SD
  s += String(XTSD::logSuccess);
  return s;
}

void printDebug() {
  String s;

  // altimeters
  s += "MS temp: " + String(MS::temp, 2) + " ˚C, pressure: " + String(MS::pressure, 2) + " mbar, alt: " + String(MS::altitude, 2) + " m\n";
  // ADS8688 PT readings
  s += "ADS8688 ch0 = " + String(ADS8688_PT::pt0, 3) + ", ch1 = " + String(ADS8688_PT::pt1, 3);
  s += ", ch2 = " + String(ADS8688_PT::pt2, 3) + ", ch3 = " + String(ADS8688_PT::pt3, 3) + "\n";
  // ADS1256 load cell readings
  s += "loadcell voltage = " + String(ADS1256_LOAD::voltageValue, 4) += " V, weight = " + String(ADS1256_LOAD::weight, 8) + " lb\n";
  // INA
  s += "ina bus voltage: " + String(INA::bus_voltage, 3);

  s += "\n\n";
  Serial.println(s);
}

void setup() {
  Serial.begin(115200);

  // set up SPI bus and CS pins
  HAL::initVSPI();
  HAL::initHSPI();
  HAL::initCSPins();

  // initialize sensors
  INA::setupINA();
  MS::setupMS();
  
  ADS8688_PT::setupADS8688();
  ADS1256_LOAD::setupADS();
  // XTSD::setupSD();
}

void loop() {
  // read sensors
  INA::readINA();
  MS::readAltimeter();
  ADS8688_PT::readADS();
  ADS1256_LOAD::readADS();

  /* SD logging */
  // int oldTime = micros();
  // XTSD::logStr = getSDstr();
  // XTSD::logSD(XTSD::logStr);
  // XTSD::logTime = micros() - oldTime;

  /* DEBUG */
  printDebug();
  // Serial.println(getSDstr());
  delay(100);
}


