#include "Globals.h"
#include "HAL.h"
#include "MS.h"
#include "XTSD.h"
#include "INA.h"
#include "ADS8688.h"
#include "ADS1256.h"
#include "CANTX.h"
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

String press_check_string() {
  String s;

  // ADS8688 PT voltage readings
  s += "ADS8688 ch0 = " + String(ADS8688_PT::pt0, 3) + ", ch1 = " + String(ADS8688_PT::pt1, 3);
  s += ", ch2 = " + String(ADS8688_PT::pt2, 3) + ", ch3 = " + String(ADS8688_PT::pt3, 3) + "\n";

  // PT calibrated
  s += "ox pt (ch0) = " + String(ADS8688_PT::ox_pt, 3) + ", press pt (ch1) = " + String(ADS8688_PT::press_pt, 3);
  s += ", fuel pt (ch2) = " + String(ADS8688_PT::fuel_pt, 3) + ", cc pt (ch3) = " + String(ADS8688_PT::cc_pt, 3) + "\n";

  // INA
  s += "ina bus voltage: " + String(INA::bus_voltage, 3);

  s += "\n\n";
  return s;
}

void printDebug() {
  String s;

  // altimeters
  s += "MS temp: " + String(MS::temp, 2) + " ˚C, pressure: " + String(MS::pressure, 2) + " mbar, alt: " + String(MS::altitude, 2) + " m\n";
  // ADS8688 PT voltage readings
  s += "ADS8688 ch0 = " + String(ADS8688_PT::pt0, 3) + ", ch1 = " + String(ADS8688_PT::pt1, 3);
  s += ", ch2 = " + String(ADS8688_PT::pt2, 3) + ", ch3 = " + String(ADS8688_PT::pt3, 3) + "\n";

  // PT calibrated
  s += "ox pt (ch0) = " + String(ADS8688_PT::ox_pt, 3) + ", press pt (ch1) = " + String(ADS8688_PT::press_pt, 3);
  s += ", fuel pt (ch2) = " + String(ADS8688_PT::fuel_pt, 3) + ", cc pt (ch3) = " + String(ADS8688_PT::cc_pt, 3) + "\n";

  // ADS1256 load cell readings
  s += "load cell weight = " + String(ADS1256_LOAD::weight, 8) + " lbf\n";
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
  CANTX::setupCAN();
  // XTSD::setupSD();
}

// core 1 processing
void loop() {
  long currentTime = millis();

  // read sensors
  INA::readINA(currentTime);            // 11k
  MS::readAltimeter(currentTime);       // 600 Hz
  ADS8688_PT::readADS(currentTime);     // 1k-2k
  ADS1256_LOAD::readADS();              // 1.3k
  // String str = getSDstr();

  String str = press_check_string();

  // /* SD logging */
  // int oldTime = micros();
  // XTSD::logStr = str;
  // XTSD::logSD(XTSD::logStr);
  // XTSD::logTime = micros() - oldTime
  

  // transmit CAN every second
  if (currentTime - CANTX::lastTransmission > 997) {
    CANTX::lastTransmission = currentTime;
    CANTX::encodeMessage(str);
  }
  
  /* DEBUG */
  // printDebug();
  Serial.println(str);
  delay(50);
}


