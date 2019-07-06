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
#include "Ref.h"
#include <unistd.h>
#include <string>
#include <memory>

// filter
#include "Filter.cpp"

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
    float doFilter(float value, float last_value)
    {
        value = (last_value * w0Te + value) / (w0Te + 1);
        return value;
    }

    std::unique_ptr<InertialSensor> get_inertial_sensor(std::string sensor_name)
    {
        if (sensor_name == "mpu")
        {
            FileManagement::Log("IMU", "Selected: MPU9250");
            auto ptr = std::unique_ptr<InertialSensor>{new MPU9250()};
            return ptr;
        }
        else if (sensor_name == "lsm")
        {
            FileManagement::Log("IMU", "Selected: LSM9DS1");
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

        int samples = 600; // user defined ratio : TODO : see what is the minimum value

        for (size_t i = 0; i < samples; i++) // add all the samples
        {
            update();
            for (size_t k = 0; k < AXIS_NB; k++)
            {
                accOffset[k] += imu_values[k];
                gyrOffset[k] += imu_values[k + ARR_GYRO_POS];
            }
            accOffset[z] -= g;

            usleep(500);
        }

        FileManagement::Log("IMU", "offsets :");
        for (size_t i = 0; i < AXIS_NB; i++) // divide and display the offsets
        {
            accOffset[i] /= samples;
            gyrOffset[i] /= samples;
            FileManagement::Log("IMU", "scaled  i : |" + to_string(i) + "| acc : " + to_string(accOffset[i]) + " gyro : " + to_string(gyrOffset[i]));
           }

        offsetSet = true;
    }

    IMU()
    {
        if (!sensor)
        {
            FileManagement::Log("IMU","Wrong sensor name. Select: mpu or lsm\n");
            //return EXIT_FAILURE;
        }

        if (!sensor->probe())
        {
            FileManagement::Log("IMU", "Sensor not enabled");
            //return EXIT_FAILURE;
        }
        sensor->initialize();

        computeOffsets();
    }

    void setMode(Data& data){
       filterUsage = data.status[status_filter_mode];
    }

    void update()
    {
        sensor->update();
        sensor->read_accelerometer(&imu_values[0], &imu_values[1], &imu_values[2]);
        sensor->read_gyroscope(&imu_values[3], &imu_values[4], &imu_values[5]);
        sensor->read_magnetometer(&imu_values[6], &imu_values[7], &imu_values[8]);


        /*
            IMU offsets and filter
        */
        for (size_t i = 0; i < AXIS_NB && offsetSet; i++) // calculate them if ofset are set
        {
            imu_values[i] -= accOffset[i];
            imu_values[i + ARR_GYRO_POS] -= gyrOffset[i];

            if (filterUsage == IMU_Filter_usage_gyr || filterUsage == IMU_Filter_usage_both)
            {
                imu_values[i + ARR_GYRO_POS] = Filter::low_pass_filter(imu_values[i + ARR_GYRO_POS], rates_old[i]);
                rates_old[i] = imu_values[i + ARR_GYRO_POS];
            }
            if (filterUsage == IMU_Filter_usage_acc || filterUsage == IMU_Filter_usage_both)
            {
                imu_values[i] = Filter::low_pass_filter(imu_values[i], accel_old[i]);
                accel_old[i] = imu_values[i];
            }
        }
    }

    void setDt(float dt_in)
    {
        dt = dt_in;
    }

    void getAngleAccel(Data& data)
    {

        if (imu_values[z] != 0)
        {
            data.ang[x] = atan(imu_values[x] / imu_values[z]) * 180 / PI;
            data.ang[y] = atan(imu_values[y] / imu_values[z]) * 180 / PI; // convert in degrees
        }
        else
        { // it really mean that we cannot determine angles this way
            data.ang[x] = 0;
            data.ang[y] = 0;
        }

        // non trivial rules

        if (imu_values[z] <= 0 && imu_values[y] <= 0)
        {
            data.ang[x] -= 180;
        }
        else if (imu_values[z] <= 0 && imu_values[y] > 0)
        {
            data.ang[x] += 180;
        }

        if (imu_values[z] <= 0 && imu_values[x] <= 0)
        {
            data.ang[y] -= 180;
        }
        else if (imu_values[z] <= 0 && imu_values[x] > 0)
        {
            data.ang[y] += 180;
        }
    }

    void getAcceleration(Data& data)
    {
        for (size_t i = 0; i < 3; i++)
        {
            data.acceleration[i] = imu_values[i]; // acceleration
        }
    }

    void getRates(Data& data) // rates
    {
        for (size_t i = 0; i < 3; i++)
        {
            data.rates[i] = imu_values[i + ARR_GYRO_POS];
        }
    }

    void getGyrationAngle(Data& data) // should be used only once
    {
        for (size_t i = 0; i < 3; i++)
        {
            data.ang[i] += imu_values[i + ARR_GYRO_POS] * dt; // to use gyration
        }
    }

    void getComplementar(Data& data) // store complementar angle in data
    {
        getAngleAccel(data);
        for (size_t i = 0; i < 3; i++)
        { // to use gyration
            data.ang[i] = (data.ang[i] + imu_values[i + ARR_GYRO_POS] * dt) * kGyr + data.ang_acc[i] * kAcc;
            if (false && data.ang[i] > 180)
            {
                data.ang[i] -= 180;
            }
            else if (false && data.ang[i] < -180)
            {
                data.ang[i] += 180;
            }
        }
    }

    void setFilterUsage(int usage)
    {
        filterUsage = usage;
    }

    // return filter usage
    int getFilterUsage()
    {
        return filterUsage;
    }
};