/*!
 * @file GPS.h
 *
 * Sparkfun U-Blox GNSS Breakout board
 *
 * Written for reading latitude and longitude via UBX binary commands over SPI.
 *
 * License: MIT, Written by Andrew Berridge, Adapted by Sanjana Kale for Rocket Project at UCLA.
 *
 */

#include <SPI.h> //Needed for SPI to GNSS
#include "HAL.h"

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS

namespace GPS {
SFE_UBLOX_GNSS myGNSS;

// #########################################

// Instantiate an instance of the SPI class. 
// Your configuration may be different, depending on the microcontroller you are using!

// HSPI
const uint8_t HSCK_PIN = 14;
const uint8_t HMISO_PIN = 34;
const uint8_t HMOSI_PIN = 13;
// #########################################

const uint8_t csPin = 27; // On ATmega328 boards, SPI Chip Select is usually pin 10. Change this to match your board.

// #########################################

long lastTime = 0; //Simple local timer. Limits amount of SPI traffic to u-blox module.
float latitude = 0;
float longitude = 0;
float altitude = 0;
byte SIV = 0;

void setup()
{

  //myGNSS.enableDebugging(); // Uncomment this line to see helpful debug messages on Serial

  // Connect to the u-blox module using SPI port, csPin and speed setting
  // ublox devices generally work up to 5MHz. We'll use 4MHz for this example:
  if (myGNSS.begin(*HAL::HSPI_bus, csPin, 4000000) == false) 
  {
    Serial.println(F("u-blox GNSS not detected on SPI bus. Please check wiring. Freezing."));
    while (1);
  }
  
  //myGNSS.factoryDefault(); delay(5000); // Uncomment this line to reset the module back to its factory defaults

  myGNSS.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX); //Set the SPI port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
}

void readGPS()
{
  //Query module only every second. Doing it more often will just cause SPI traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 2000)
  {
    lastTime = millis(); //Update the timer
    
    latitude = myGNSS.getLatitude() / 1E7;
    // Serial.print(F("Lat: "));
    // Serial.print(latitude);

    longitude = myGNSS.getLongitude() / 1E7;
    DEBUG(F(" Long: "));
    DEBUG(longitude);
    DEBUG(F(" (degrees * 10^-7)"));

    altitude = myGNSS.getAltitude() / 1E3;
    DEBUG(F(" Alt: "));
    DEBUG(altitude);
    DEBUG(F(" (mm)"));

    SIV = myGNSS.getSIV();
    DEBUG(F(" SIV: "));
    DEBUG(SIV);

    // Serial.println();
  }
}
}