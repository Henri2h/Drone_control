/*
Provided to you by Emlid Ltd (c) 2015.
twitter.com/emlidtech || www.emlid.com || info@emlid.com

Example: Read accelerometer, gyroscope and magnetometer values from
inertial measurement unit: MPU9250 or LSM9DS1 over SPI on Raspberry Pi + Navio.

Navio's onboard sensors are connected to the SPI bus on Raspberry Pi
and can be read through /dev/spidev0.1 (MPU9250), /dev/spidev0.3 (acc/gyro LSM9DS1)
and /dev/spidev0.2 (mag LSM9DS1).

To run this example navigate to the directory containing it and run following commands:
make
./AccelGyroMag -i [sensor name]
Sensors names: mpu is MPU9250, lsm is LSM9DS1.
For print help:
./AccelGyroMag -h
*/

#include "Common/MPU9250.h"
#include "Navio2/LSM9DS1.h"
#include "Common/Util.h"
#include <unistd.h>
#include <string>
#include <memory>



class IMU{
    std::unique_ptr <InertialSensor> get_inertial_sensor( std::string sensor_name)
    {
        if (sensor_name == "mpu") {
            printf("Selected: MPU9250\n");
            auto ptr = std::unique_ptr <InertialSensor>{ new MPU9250() };
            return ptr;
        }
        else if (sensor_name == "lsm") {
            printf("Selected: LSM9DS1\n");
            auto ptr = std::unique_ptr <InertialSensor>{ new LSM9DS1() };
            return ptr;
        }
        else {
            return NULL;
        }
    }
    
    std::unique_ptr <InertialSensor> sensor = get_inertial_sensor("lsm"); // mpu or lsm

    public:
    IMU(){
        if (!sensor) {
            printf("Wrong sensor name. Select: mpu or lsm\n");
            //return EXIT_FAILURE;
            }
        
        if (!sensor->probe()) {
            printf("Sensor not enabled\n");
            //return EXIT_FAILURE;
        }
        sensor->initialize();
    }
   

    float * read()
    {
        static float values[9];
        
        sensor->update();
        sensor->read_accelerometer(&values[0], &values[1], &values[2]);
        sensor->read_gyroscope(&values[3], &values[4], &values[5]);
        sensor->read_magnetometer(&values[6], &values[7], &values[8]);

        return values;
    }
};