#ifndef ICM_H
#define ICM_H

#include "HAL.h"

#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>

namespace ICM {

    Adafruit_ICM20948 imu;

    double IMU_temp;
    double accel_x, accel_y, accel_z;
    double gyro_x, gyro_y, gyro_z;

    void setupIMU() {
        for (int i=0; i<3000; i++) {
            if(imu.begin_SPI(HAL::ICM20948_CS, HAL::VSPI_bus)) {
                DEBUGLN("found");
                break;
            }
            DEBUGLN("Failed to find imu20948 chip");
        }

        
        // imu.setAccelRange(imu20948_ACCEL_RANGE_16_G);
        DEBUG("Accelerometer range set to: ");
        switch (imu.getAccelRange()) {
        case ICM20948_ACCEL_RANGE_2_G:
            DEBUGLN("+-2G");
            break;
        case ICM20948_ACCEL_RANGE_4_G:
            DEBUGLN("+-4G");
            break;
        case ICM20948_ACCEL_RANGE_8_G:
            DEBUGLN("+-8G");
            break;
        case ICM20948_ACCEL_RANGE_16_G:
            DEBUGLN("+-16G");
            break;
        }
        DEBUGLN("OK");

        // imu.setGyroRange(imu20948_GYRO_RANGE_2000_DPS);
        DEBUG("Gyro range set to: ");
        switch (imu.getGyroRange()) {
        case ICM20948_GYRO_RANGE_250_DPS:
            DEBUGLN("250 degrees/s");
            break;
        case ICM20948_GYRO_RANGE_500_DPS:
            DEBUGLN("500 degrees/s");
            break;
        case ICM20948_GYRO_RANGE_1000_DPS:
            DEBUGLN("1000 degrees/s");
            break;
        case ICM20948_GYRO_RANGE_2000_DPS:
            DEBUGLN("2000 degrees/s");
            break;
        }

        //  imu.setAccelRateDivisor(4095);
        uint16_t accel_divisor = imu.getAccelRateDivisor();
        float accel_rate = 1125 / (1.0 + accel_divisor);

        DEBUG("Accelerometer data rate divisor set to: ");
        DEBUGLN(accel_divisor);
        DEBUG("Accelerometer data rate (Hz) is approximately: ");
        DEBUGLN(accel_rate);

        //  imu.setGyroRateDivisor(255);
        uint8_t gyro_divisor = imu.getGyroRateDivisor();
        float gyro_rate = 1100 / (1.0 + gyro_divisor);

        DEBUG("Gyro data rate divisor set to: ");
        DEBUGLN(gyro_divisor);
        DEBUG("Gyro data rate (Hz) is approximately: ");
        DEBUGLN(gyro_rate);
    }
    
    void readIMU() {
    
        /* Get a new normalized sensor event */
        sensors_event_t accel;
        sensors_event_t gyro;
        sensors_event_t temp;
        imu.getEvent(&accel, &gyro, &temp);

        IMU_temp = temp.temperature;
        accel_x = accel.acceleration.x;
        accel_y = accel.acceleration.y;
        accel_z = accel.acceleration.z;
        gyro_x = gyro.gyro.x;
        gyro_y = gyro.gyro.y;
        gyro_z = gyro.gyro.z;
        // DEBUGLN("read");
        
        DEBUG("Temp:");
        DEBUG(IMU_temp);
        DEBUG("*C, ");


        DEBUG("\t\tAccel X: ");
        DEBUG(accel_x);
        DEBUG(", Y: ");
        DEBUG(accel_y);
        DEBUG(", Z: ");
        DEBUG(accel_z);
        DEBUG(" m/s^2, ");

        DEBUG("\t\tGyro X: ");
        DEBUG(gyro_x);
        DEBUG(", Y: ");
        DEBUG(gyro_y);
        DEBUG(", Z: ");
        DEBUG(gyro_z);
        DEBUGLN(" radians/s");
    }
}

#endif