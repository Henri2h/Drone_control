#include <Common/MS5611.h>
#include <Common/Util.h>
#include <unistd.h>
#include <stdio.h>

class Barometer
{
    float *status;
    MS5611 barometer;

  public:
    void initialize(float *status)
    {
        this->status = status;

        barometer.initialize();
    }

    void update()
    {
        barometer.refreshPressure();
        usleep(10000); // Waiting for pressure data ready
        barometer.readPressure();

        barometer.refreshTemperature();
        usleep(10000); // Waiting for temperature data ready
        barometer.readTemperature();

        barometer.calculatePressureAndTemperature();

        status[status_baro_pressure] = barometer.getPressure();
        status[status_baro_temperature] = barometer.getPressure();
    }
};