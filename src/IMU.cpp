#include "IMU.h"

float IMU::doFilter(float value, float last_value)
{
    value = (last_value * w0Te + value) / (w0Te + 1);
    return value;
}

std::unique_ptr<InertialSensor> IMU::get_inertial_sensor(std::string sensor_name)
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

void IMU::computeOffsets()
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

IMU::IMU()
{
    if (!sensor)
    {
        FileManagement::Log("IMU", "Wrong sensor name. Select: mpu or lsm\n");
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

void IMU::setMode(Data &data)
{
    
    //data.status[status_filter_mode] = IMU_Filter_usage_gyr;
    filterUsage = data.status[status_filter_mode];
    
}

void IMU::update()
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

void IMU::setDt(float dt_in)
{
    dt = dt_in;
}

void IMU::getAngleAccel(Data &data)
{

    if (imu_values[z] != 0)
    {
        data.ang_acc[x] = atan(imu_values[x] / imu_values[z]) * 180 / PI;
        data.ang_acc[y] = atan(imu_values[y] / imu_values[z]) * 180 / PI; // convert in degrees
    }
    else
    { // it really mean that we cannot determine angles this way
        data.ang_acc[x] = 0;
        data.ang_acc[y] = 0;
    }

    // non trivial rules

    if (imu_values[z] <= 0 && imu_values[y] <= 0)
    {
        data.ang_acc[x] -= 180;
    }
    else if (imu_values[z] <= 0 && imu_values[y] > 0)
    {
        data.ang_acc[x] += 180;
    }

    if (imu_values[z] <= 0 && imu_values[x] <= 0)
    {
        data.ang_acc[y] -= 180;
    }
    else if (imu_values[z] <= 0 && imu_values[x] > 0)
    {
        data.ang_acc[y] += 180;
    }
}

void IMU::getAcceleration(Data &data)
{
    for (size_t i = 0; i < 3; i++)
    {
        data.acceleration[i] = imu_values[i]; // acceleration
    }
}

void IMU::getRates(Data &data) // rates
{
    for (size_t i = 0; i < 3; i++)
    {
        data.rates[i] = imu_values[i + ARR_GYRO_POS];
    }
}

void IMU::getGyrationAngle(Data &data) // should be used only once
{
    for (size_t i = 0; i < 3; i++)
    {
        data.ang[i] += imu_values[i + ARR_GYRO_POS] * dt; // to use gyration
    }
}

void IMU::getComplementar(Data &data) // store complementar angle in data
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

void IMU::setFilterUsage(int usage)
{
    filterUsage = usage;
}

// return filter usage
int IMU::getFilterUsage()
{
    return filterUsage;
}
