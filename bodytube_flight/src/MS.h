#ifndef MS_H
#define MS_H

#include "HAL.h"
#include "../lib/MS5607/MS5611_SPI2.h"

namespace MS {
    
    // RP_MS5607 ms;
    MS5611_SPI ms5607(HAL::MS5607_CS, HAL::VSPI_bus);

    // MS5607 ms5607;
    
    uint32_t start, stop;
    
    float temp, pressure, altitude;

    // #define SEALEVELPRESSURE_HPA 1018.90 // Sea level pressure of LA 
    const float SEALEVELPRESSURE_HPA = 1018.7; // Sea level pressure of LA 

    const float R = 287.052; // specific gas constant R*/M0
    const float g = 9.80665; // standard gravity 
    const float t_grad = 0.0065; // gradient of temperature
    const float t0 = 273.15 + 15; // temperature at 0 altitude
    

    void setupMS() {

        // ms5607.begin();
        ms5607.setSPIport(HAL::HSPI_bus);

        // SPI.begin();

        if (ms5607.begin() == true)
        {
            DEBUG("MS5607 found: ");
            Serial.println(ms5607.getDeviceID(), HEX);
        }

        if (ms5607.reset(1) == true)
        {
            DEBUG("MS5611 conversion set to MS5607");
        }
        
    }


    
    int readAltimeter() {
        
        // if(ms5607.readDigitalValue()){
        //     temp = ms5607.getTemperature();
        //     pressure = ms5607.getPressure();
        //     altitude = ms5607.getAltitude();
        // }else{
        //     DEBUGLN("Error in reading digital value in sensor!");
        //     return 0;
        // }     

        // DEBUG("Altimeter 2: ");
        // DEBUG(temp);
        // DEBUG(" *C, ");
        // DEBUG(pressure);
        // DEBUG(" kPa, ");
        // DEBUG(altitude);
        // DEBUGLN(" m");
        
        // return 1;

        ms5607.read();
        temp = ms5607.getTemperature();
        pressure = ms5607.getPressure();

        // TODO: potentially sus conversion?
        altitude = t0 / t_grad * (1 - exp((t_grad * R / g) * log(pressure / SEALEVELPRESSURE_HPA)));


        DEBUG("Altimeter 2 (MS5607): ");
        DEBUG(temp);
        // DEBUG(",");
        DEBUG(" *C, ");
        DEBUG(pressure);
        // DEBUG(",");
        DEBUG(" mbar, ");
        DEBUGLN(altitude);
        DEBUGLN(" m");
        return 1;
    }

    

}


#endif