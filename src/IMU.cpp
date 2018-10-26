/*
Provided to you by Emlid Ltd (c) 2015.
twitter.com/emlidtech || www.emlid.com || info@emlid.com

Navio's onboard sensors are connected to the SPI bus on Raspberry Pi
and can be read through /dev/spidev0.1 (MPU9250), /dev/spidev0.3 (acc/gyro LSM9DS1)
and /dev/spidev0.2 (mag LSM9DS1).


All angle are expressed in degrees
*/

#include "Common/MPU9250.h"
#include "Navio2/LSM9DS1.h"
#include "Common/Util.h"
#include <unistd.h>
#include <string>
#include <memory>

#define AXIS_NB 3       // three axis
#define ARR_ACCEL_POS 0 // position of accel data in array
#define ARR_GYRO_POS 1  // position of gyro data in array
#define ARR_MAG_POS 2   // position of mag data in array

class IMU
{
    float dt;

    const int x = 0;
    const int y = 1;
    const int z = 2;
    const float PI = 3.1415926535;

    const float kAcc = 0.02;
    const float kGyr = 0.98;

    float imu_values[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::unique_ptr<InertialSensor> get_inertial_sensor(std::string sensor_name)
    {
        if (sensor_name == "mpu")
        {
            printf("Selected: MPU9250\n");
            auto ptr = std::unique_ptr<InertialSensor>{new MPU9250()};
            return ptr;
        }
        else if (sensor_name == "lsm")
        {
            printf("Selected: LSM9DS1\n");
            auto ptr = std::unique_ptr<InertialSensor>{new LSM9DS1()};
            return ptr;
        }
        else
        {
            return NULL;
        }
    }

    std::unique_ptr<InertialSensor> sensor = get_inertial_sensor("lsm"); // mpu or lsm

  public:
    void computeOffsets()
    {
        update();

        int samples = 200;
        float accRaw[AXIS_NB][samples];
        float gyrRaw[AXIS_NB][samples];

        for (size_t i = 0; i < samples; i++)
        {
            update();

            for (size_t k = 0; i < AXIS_NB; i++)
            {
                accRaw[k][i] = imu_values[k];
                gyrRaw[k][i] = imu_values[k + ARR_GYRO_POS];
            }
        }

        float gyrOffset[3] = {0, 0, 0};
        float accOffset[3] = {0, 0, 0};

        for (size_t i = 0; i < samples; i++) // add all the samples
        {

            for (size_t k = 0; i < AXIS_NB; i++)
            {
                accOffset[k] += accRaw[k][i];
                gyrOffset[k] += gyrRaw[k][i];
            }
        }

        std::cout << "IMU offsets : ";
        for (size_t i = 0; i < AXIS_NB; i++) // divide and display the offsets
        {
            accOffset[i] /= samples;
            gyrOffset[i] /= samples;

            std::cout << "  i : |" << i << "| acc : " << accOffset[i] << " gyro : " << gyrOffset[i] << " ";
        }
        std::cout << "\n";
    }

    IMU()
    {
        if (!sensor)
        {
            printf("Wrong sensor name. Select: mpu or lsm\n");
            //return EXIT_FAILURE;
        }

        if (!sensor->probe())
        {
            printf("Sensor not enabled\n");
            //return EXIT_FAILURE;
        }
        sensor->initialize();

        computeOffsets();
    }

    void update()
    {
        sensor->update();
        sensor->read_accelerometer(&imu_values[0], &imu_values[1], &imu_values[2]);
        sensor->read_gyroscope(&imu_values[3], &imu_values[4], &imu_values[5]);
        sensor->read_magnetometer(&imu_values[6], &imu_values[7], &imu_values[8]);

        // convert radian to degrees:

        for (size_t i = ARR_GYRO_POS; i < ARR_GYRO_POS + AXIS_NB; i++)
        {
            imu_values[i] *= 180 / PI;
        }
    }

    void setDt(float dt_in)
    {
        dt = dt_in;
    }

    float *getAngleAccel()
    {
        static float ang[3] = {0, 0, 0};

        if (imu_values[z] != 0)
        {
            ang[x] = atan(imu_values[x] / imu_values[z]) * 180 / PI;
            ang[y] = atan(imu_values[y] / imu_values[z]) * 180 / PI; // convert in degrees
        }
        else
        { // it really mean that we cannot determine angles this way
            ang[x] = 0;
            ang[y] = 0;
        }

        // non trivial rules
        /*
        if(imu_values[z] <= 0 && imu_values[y] <= 0){ ang[x] -= 180; }
        else if (imu_values[z] <=0 && imu_values[y] > 0){ ang[x] += 180; }

        if(imu_values[z] <= 0 && imu_values[x] <= 0){ ang[y] -= 180; }
        else if (imu_values[z] <= 0 && imu_values[x] > 0){ ang[y] += 180; }
        */

        return ang;
    }

    float *getGyrationAngle(float ang[])
    {
        for (size_t i = 0; i < 3; i++)
        {
            ang[i] += imu_values[i + ARR_GYRO_POS] * dt; // to use gyration
        }
        return ang;
    }

    float *getRates(float *rates)
    {

        for (size_t i = 0; i < 3; i++)
        {
            rates[i] = imu_values[i + ARR_GYRO_POS];
        }
    }

    float *getComplementar(float *ang)
    {
        for (size_t i = 0; i < 3; i++)
        { // to use gyration
            ang[i] = (ang[i] + imu_values[i + ARR_GYRO_POS] * dt) * kGyr + imu_values[i] * kAcc;
        }
        return ang;
    }
};