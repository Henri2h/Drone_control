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
#define ARR_GYRO_POS 3  // position of gyro data in array
#define ARR_MAG_POS 6   // position of mag data in array

#define PI 3.1415926535

class IMU
{

    double gyrOffset[3] = {0, 0, 0};
    double accOffset[3] = {0, 0, 0};
    bool offsetSet = false;

    float dt;

    const int x = 0;
    const int y = 1;
    const int z = 2;

    const float kAcc = 0;
    const float kGyr = 1;

    const float g = 9.81;

    float imu_values[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::unique_ptr<InertialSensor> get_inertial_sensor(std::string sensor_name)
    {
        if (sensor_name == "mpu")
        {
            printf("[ IMU ] : Selected: MPU9250\n");
            auto ptr = std::unique_ptr<InertialSensor>{new MPU9250()};
            return ptr;
        }
        else if (sensor_name == "lsm")
        {
            printf("[ IMU ] : Selected: LSM9DS1\n");
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
        offsetSet = false;
        for (size_t i = 0; i < AXIS_NB; i++)
        {
            gyrOffset[i] = 0;
            accOffset[i] = 0;
        }

        int samples = 900;

        for (size_t i = 0; i < samples; i++) // add all the samples
        {
            update();
            for (size_t k = 0; k < AXIS_NB; k++)
            {
                accOffset[k] += imu_values[k];
                gyrOffset[k] += imu_values[k + ARR_GYRO_POS];
            }
            accOffset[z] -= g;

            usleep(50);
        }

        std::cout << "[ IMU ] : offsets : \n";
        for (size_t i = 0; i < AXIS_NB; i++) // divide and display the offsets
        {
            accOffset[i] /= samples;
            gyrOffset[i] /= samples;

            std::cout << "[ IMU ] : scaled  i : |" << i << "| acc : " << accOffset[i] << " gyro : " << gyrOffset[i] << "\n";
        }

        offsetSet = true;
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

        for (size_t i = 0; i < AXIS_NB && offsetSet; i++) // calculate them if ofset are set
        {
            imu_values[i] -= accOffset[i];
            imu_values[i + ARR_GYRO_POS] -= gyrOffset[i];
        }
    }

    void setDt(float dt_in)
    {
        dt = dt_in;
    }

    void getAngleAccel(float *ang)
    {

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
        
        if(imu_values[z] <= 0 && imu_values[y] <= 0){ ang[x] -= 180; }
        else if (imu_values[z] <=0 && imu_values[y] > 0){ ang[x] += 180; }

        if(imu_values[z] <= 0 && imu_values[x] <= 0){ ang[y] -= 180; }
        else if (imu_values[z] <= 0 && imu_values[x] > 0){ ang[y] += 180; }
        
    }

    void getAcceleration(float *ang)
    {
        for (size_t i = 0; i < 3; i++)
        {
            ang[i] = imu_values[i]; // acceleration
        }
    }

    void getRates(float *rates) // rates
    {
        for (size_t i = 0; i < 3; i++)
        {
            rates[i] = imu_values[i + ARR_GYRO_POS];
        }
    }

    void getGyrationAngle(float *ang)
    {
        for (size_t i = 0; i < 3; i++)
        {
            ang[i] += imu_values[i + ARR_GYRO_POS] * dt; // to use gyration
        }
    }

    float *getComplementar(float *ang, float *ang_acc)
    {
        getAngleAccel(ang_acc);
        for (size_t i = 0; i < 3; i++)
        { // to use gyration
            ang[i] = (ang[i] + imu_values[i + ARR_GYRO_POS] * dt) * kGyr + ang_acc[i] * kAcc;
            if(false && ang[i] > 180){
                ang[i] -= 180;
            }
            else if (false && ang[i] < -180){
                ang[i] += 180;
            }
        }
        return ang;
    }
};