#ifndef BMP_H
#define BMP_H

#include "HAL.h"
#include <Adafruit_BMP3XX.h>


namespace BMP {
    
    Adafruit_BMP3XX bmp1;
    // Adafruit_BMP3XX bmp2;

    // const float SEALEVELPRESSURE_HPA = 1016.30; // Sea level pressure of LA 
    const float SEALEVELPRESSURE_HPA = 1018.7; // Sea level pressure of LA 

    double temp1, pressure1, altitude1;
    // double temp2, pressure2, altitude2;

    void setupBMP() {
        for (int i=0; i<300; i++) {
            if(bmp1.begin_SPI(HAL::ALT1_BMP390_CS, HAL::HSPI_bus))
                break;    
            DEBUGLN("Failed to find BMP 1");
        }

        // for (int i=0; i<3000; i++) {
        //     if(bmp2.begin_SPI(HAL::ALTIMETER2_CS, HAL::sensorSPI))
        //         break;
        //     DEBUGLN("Failed to find BMP 2");
        // }

        // bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
        // bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
        // bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
        // bmp.setOutputDataRate(BMP3_ODR_50_HZ);
    }


    
    int readAltimeters() {
  
        if (! bmp1.performReading()) {
            return 0;
        }
        else {
            temp1 = bmp1.temperature;
            pressure1 = bmp1.pressure / 100.0;
            altitude1 = bmp1.readAltitude(SEALEVELPRESSURE_HPA);
        }

        // if (! bmp2.performReading()) {
        //     return 0;
        // }
        // else {
        //     temp2 = bmp2.temperature;
        //     pressure2 = bmp2.pressure / 1000.0;
        //     altitude2 = bmp2.readAltitude(SEALEVELPRESSURE_HPA);
        // }

        DEBUG("Altimeter 1 (BMP): ");
        DEBUG(temp1);
        DEBUG(" *C, ");
        DEBUG(pressure1);
        DEBUG(" mBar, ");
        DEBUG(altitude1);
        DEBUGLN(" m");
        // DEBUG("Altimeter 2: ");
        // DEBUG(temp2);
        // DEBUG(" *C, ");
        // DEBUG(pressure2);
        // DEBUG(" kPa, ");
        // DEBUG(altitude2);
        // DEBUGLN(" m");

        return 1;

    }

    

}


#endif