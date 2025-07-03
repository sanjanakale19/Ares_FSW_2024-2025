#include "Globals.h"
#include "HAL.h"
#include "GPS.h"
#include "BMP.h"
#include "ICM.h"
#include "XTSD.h"
#include "MS.h"
#include "INA.h"
#include "Radio.h"
#include "INA.h"
#include "CANRX.h"
#include <Arduino.h>

String getSDstring() {
  String s = (String)micros() + ",";
  s += String(XTSD::logTime) + ",";
  // altimeters
  s += String(MS::temp, 2) + "," + String(MS::pressure, 2) + "," + String(MS::altitude, 2) + ",";
  s += String(BMP::temp1, 2) + "," + String(BMP::pressure1, 2) + "," + String(BMP::altitude1, 2) + ",";
  // IMUs
  s += String(ICM::accel_x, 2) + "," + String(ICM::accel_y, 2) + "," + String(ICM::accel_z, 2) + ",";
  s += String(ICM::gyro_x, 2) + "," + String(ICM::gyro_y, 2) + "," + String(ICM::gyro_z, 2) + ",";
  // GPS
  s += String(GPS::latitude, 6) + "," + String(GPS::longitude, 6) + "," + String(GPS::altitude, 2) + "," + String(GPS::SIV) + ",";
  // INA
  s += String(INA::bus_voltage, 3);

  return s;
}

void printDebug() {
  String s;

  // altimeters
  s += "MS temp: " + String(MS::temp, 2) + ", pressure: " + String(MS::pressure, 2) + ", alt: " + String(MS::altitude, 2) + "\n";
  s += "BMP temp: " + String(BMP::temp1, 2) + ", pressure: " + String(BMP::pressure1, 2) + ", alt: " + String(BMP::altitude1, 2) + "\n";
  // IMU
  s += "ICM accelx: " + String(ICM::accel_x, 2) + ", accely: " + String(ICM::accel_y, 2) + ", accelz: " + String(ICM::accel_z, 2) + ", ";
  s += "gyro_x: " + String(ICM::gyro_x, 2) + ", gyro_y: " + String(ICM::gyro_y, 2) + ", gyro_z: " + String(ICM::gyro_z, 2) + "\n";
  // GPS
  s += "gps lat: " + String(GPS::latitude, 6) + ", gps long: " + String(GPS::longitude, 6) + ", gps alt: " + String(GPS::altitude, 2) + ", ";
  s += "gps SIV: " + String(GPS::SIV, 6) + "\n";
  // INA
  s += "ina bus voltage: " + String(INA::bus_voltage, 3);

  s += "\n\n";
  Serial.println(s);
}

// void core0_processes(void* param) {
//   while(1) {
//     // read core0/HSPI sensors
//     GPS::readGPS();
//     BMP::readAltimeters();

//     // read CAN, will wait for 20 ms, set to 0 for non blocking
//     CANRX::decodeMessage(20);

//     if (millis() - Radio::lastTransmissionTime > 250) {
//       Radio::downlink_packet = "";
//       Radio::downlink_packet += getSDstring();
//       Radio::downlink_packet += "," + String(XTSD::logSuccess) + "|";

//       if (CANRX::msgReady) {
//         Serial.println("got a complete CAN packet");
//         Radio::downlink_packet += CANRX::decoded_msg;
//         CANRX::msgReady = false;
//       }
//       Serial.println(Radio::downlink_packet);
//       Radio::transmitPacket();
//     }
//   }
// }

void core0_processes(void* param) {
  while(1) {
    long currentMillis = millis();

    // read core0/HSPI sensors
    GPS::readGPS(currentMillis);
    BMP::readAltimeters(currentMillis);

    // read CAN, will wait for 20 ms, set to 0 for non blocking
    CANRX::decodeMessage(0);

    // debug printing
    // if (CANRX::msgDecodeState == 2) {
    //   Serial.println("got a complete CAN packet");
    //   Serial.println(CANRX::decoded_msg);
    // }

    if (CANRX::msgDecodeState != 1 && (currentMillis - Radio::lastTransmissionTime > 250)) {
      Radio::downlink_packet = "";
      Radio::downlink_packet += getSDstring();
      Radio::downlink_packet += "," + String(XTSD::logSuccess) + "|";

      if (CANRX::msgDecodeState == 2) {
        // Serial.println("got a complete CAN packet");
        Radio::downlink_packet += CANRX::decoded_msg;
        CANRX::msgDecodeState = 0;
      }
      Serial.println(Radio::downlink_packet);
      Radio::transmitPacket();
    }
  }
}

void setup() {
  Serial.begin(9600);

  INA::setupINA();

  // setup VSPI, HSPI, CS pins
  HAL::initCSPins();
  HAL::initHSPI_HAL();
  HAL::initVSPI_HAL();
  
  // initialize sensors
  MS::setupMS();
  BMP::setupBMP();
  ICM::setupIMU();
  GPS::setup();
  Radio::setupRadio();
  XTSD::setupSD();
  CANRX::setupCAN();
    
  xTaskCreatePinnedToCore (
    core0_processes,     // Function to implement the task
    "core0",   // Name of the task
    10000,      // Stack size in words
    NULL,      // Task input parameter
    0,         // Priority of the task
    NULL,      // Task handle.
    0          // Core where the task should run
  );

}

void loop() {
  // read core1/VSPI sensors
  MS::readAltimeter();
  ICM::readIMU();
  INA::readINA();
  
  /* SD logging */
  int oldTime = micros();
  XTSD::logStr = getSDstring();
  XTSD::logSD(XTSD::logStr);
  XTSD::logTime = micros() - oldTime;

  /* DEBUG */
  // printDebug();
  // delay(100);
}
