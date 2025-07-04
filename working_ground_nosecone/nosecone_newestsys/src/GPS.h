/*
  Reading lat and long via UBX binary commands over SPI
  By: Andrew Berridge
  Date: 27th June 2021
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to query a u-blox module for its lat/long/altitude over SPI. We also
  turn off the NMEA output on the SPI port. This decreases the amount of SPI traffic 
  dramatically.

  Note: Long/lat are large numbers because they are * 10^7. To convert lat/long
  to something google maps understands simply divide the numbers by 10,000,000. We 
  do this so that we don't have to use floating point numbers.

  Leave NMEA parsing behind. Now you can simply ask the module for the datums you want!

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  NEO-M8U: https://www.sparkfun.com/products/16329
  NEO-M9N: https://www.sparkfun.com/products/17285

  Hardware Connections:
  You need to connect the SPI pins from your microcontroller to the specific pins on your SparkFun product. 
  Connections will vary based on your microcontroller, but for reference please refer to this tutorial on SPI:
  https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi/all
  
  Most new boards now use the terms:
  CS - Chip Select
  COPI - Controller Out, Peripheral In
  CIPO - Controller in, Peripheral Out
  SCK - Serial Clock

  You can choose any pin for Chip Select, but the others are likely either defined by the library you are using
  (see here for the standard Arduino one: https://www.arduino.cc/en/reference/SPI) or the microcontroller. The
  ESP32 has two standard, selectable SPI ports, for example.

  To enable SPI communication, you will need to solder the DSEL/SPI jumper closed on your u-blox board.

  IMPORTANT: there have been reports that some u-blox devices do not respond to the UBX protocol over SPI
  with the factory settings. You may find you need to connect to the device via USB and u-center and set
  the incoming protocol to UBX only. Make sure you disable all other protocols as inputs if you can't
  get things to work! Hopefully this is just a bug in the u-blox firmware that will be fixed soon ;-)

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