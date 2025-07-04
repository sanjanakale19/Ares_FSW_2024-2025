// #define DEBUG_MODE

#include "Globals.h"
#include "HAL.h"
#include "GPS.h"
#include "BMP.h"
#include "ICM.h"
#include "XTSD.h"
#include "MS.h"
#include "INA.h"
#include "Radio.h"
#include <Arduino.h>



void radioGPS(void* param) {
  while(1) {
    long current = millis();
    GPS::readGPS();
    BMP::readAltimeters();

    if (current - Radio::lastTransmissionTime > 500) {

      Radio::downlink_packet = millis();
      Radio::downlink_packet += "," + String(MS::temp, 2) + "," + String(MS::pressure, 2) + "," + String(MS::altitude, 2);
      Radio::downlink_packet += "," + String(BMP::temp1, 2) + "," + String(BMP::pressure1, 2) + "," + String(BMP::altitude1, 2);
      Radio::downlink_packet += "," + String(ICM::accel_x, 2) + "," + String(ICM::accel_y, 2) + "," + String(ICM::accel_z, 2);
      Radio::downlink_packet += "," + String(ICM::gyro_x, 2) + "," + String(ICM::gyro_y, 2) + "," + String(ICM::gyro_z, 2);
      Radio::downlink_packet += "," + String(GPS::latitude, 6) + "," + String(GPS::longitude, 6) + "," + String(GPS::altitude, 2) + "," + String(GPS::SIV);
      Radio::downlink_packet += "," + String(INA::bus_voltage, 3);

      Serial.println(Radio::downlink_packet);
      Radio::transmitPacket();
      Radio::lastTransmissionTime = current;
      
    }
    // DEBUGLN("r");
  }
}

void setup() {
  Serial.begin(9600);
  INA::setupINA();

  HAL::initCSPins();
  HAL::initHSPI_HAL();
  HAL::initVSPI_HAL();
  
  BMP::setupBMP();
  ICM::setupIMU();
  MS::setupMS();
  GPS::setup();
  // XTSD::setupSD();
  
  Radio::setupRadio();
  
    
  xTaskCreatePinnedToCore (
    radioGPS,     // Function to implement the task
    "radioGPS",   // Name of the task
    10000,      // Stack size in words
    NULL,      // Task input parameter
    0,         // Priority of the task
    NULL,      // Task handle.
    0          // Core where the task should run
  );

}

void loop() {
  MS::readAltimeter();
  ICM::readIMU();
  INA::readINA();
  
  int oldTime = micros();

  // for SD LOGGING
  // XTSD::logStr = (String)micros() + ",";
  // XTSD::logStr += String(XTSD::logTime) + ",";
  // XTSD::logStr += String(BMP::temp1, 2) + "," + String(BMP::pressure1, 2) + "," + String(BMP::altitude1, 2) + ",";
  // XTSD::logStr += String(MS::temp, 2) + "," + String(MS::pressure, 2) + "," + String(MS::altitude, 2) + ",";
  // XTSD::logStr += String(ICM::accel_x, 2) + "," + String(ICM::accel_y, 2) + "," + String(ICM::accel_z, 2) + ",";
  // XTSD::logStr += String(ICM::gyro_x, 2) + "," + String(ICM::gyro_y, 2) + "," + String(ICM::gyro_z, 2) + ",";
  // XTSD::logStr += String(GPS::latitude, 6) + "," + String(GPS::longitude, 6) + "," + String(GPS::altitude, 2) + ",";

  // for DEBUG
  // XTSD::logStr += "BMP temp: " + String(BMP::temp1, 2) + ", pressure: " + String(BMP::pressure1, 2) + ", alt: " + String(BMP::altitude1, 2) + ",\n";
  // XTSD::logStr += "MS temp: " + String(MS::temp, 2) + ", pressure: " + String(MS::pressure, 2) + ", alt: " + String(MS::altitude, 2) + ",\n";
  // XTSD::logStr += "ICM accelx: " + String(ICM::accel_x, 2) + ", accely: " + String(ICM::accel_y, 2) + ", accelz: " + String(ICM::accel_z, 2) + ", ";
  // XTSD::logStr += " gyro_x: " + String(ICM::gyro_x, 2) + ", gyro_y: " + String(ICM::gyro_y, 2) + ", gyro_z: " + String(ICM::gyro_z, 2) + ",\n";
  // XTSD::logStr += "gps lat: " + String(GPS::latitude, 6) + ", gps long: " + String(GPS::longitude, 6) + ", gps alt: " + String(GPS::altitude, 2) + ", ";
  // XTSD::logStr += "gps SIV: " + String(GPS::SIV, 6) + ",\n\n";

  // // XTSD::logSD(XTSD::logStr);
  // XTSD::logTime = micros() - oldTime;
  // XTSD::i++;


  // Serial.println(XTSD::logStr);

  // DEBUGLN();
  // delay(100);
}
