/*
Provided to you by Emlid Ltd (c) 2015.
twitter.com/emlidtech || www.emlid.com || info@emlid.com

Navio's onboard sensors are connected to the SPI bus on Raspberry Pi
and can be read through /dev/spidev0.1 (MPU9250), /dev/spidev0.3 (acc/gyro LSM9DS1)
and /dev/spidev0.2 (mag LSM9DS1).


All angle are expressed in degrees
*/

#ifndef IMU_H
#define IMU_H

#include "Common/MPU9250.h"
#include "Navio2/LSM9DS1.h"
#include "Common/Util.h"
#include "Ref.h"
#include <unistd.h>
#include <string>
#include <memory>
#include <math.h>

// filter
#include "Filter.h"
#include "Data.h"
#include "FileManagement.h"

#define AXIS_NB 3       // three axis
#define ARR_ACCEL_POS 0 // position of accel data in array
#define ARR_GYRO_POS 3  // position of gyro data in array
#define ARR_MAG_POS 6   // position of mag data in array

class IMU
{

    double gyrOffset[3] = {0, 0, 0};
    double accOffset[3] = {0, 0, 0};
    bool offsetSet = false;

    float dt;

    const int x = 0;
    const int y = 1;
    const int z = 2;

    const float kAcc = 0.02;
    const float kGyr = 0.98;

    const float g = 9.81;

    float imu_values[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    float accel_old[3] = {0, 0, 0};
    float rates_old[3] = {0, 0, 0};
    float w0Te = 1;
    int filterUsage = IMU_Filter_usage_both; // default value
    /*
    0 : no
    1 : acceleration
    2 : gyration
    3 : both
    */
    float doFilter(float value, float last_value);

    std::unique_ptr<InertialSensor> get_inertial_sensor(std::string sensor_name);

    std::unique_ptr<InertialSensor> sensor = get_inertial_sensor("lsm"); // mpu or lsm

public:
    IMU();

    void update();

    void setDt(float dt_in);
    void setMode(Data &data);

    void getAngleAccel(Data &data);
    void getAcceleration(Data &data);
    void getRates(Data &data);         // rates
    void getGyrationAngle(Data &data); // should be used only once
    void getComplementar(Data &data);  // store complementar angle in data
    void setFilterUsage(int usage);

    // return filter usage
    int getFilterUsage();

    void computeOffsets();
};
#endif
