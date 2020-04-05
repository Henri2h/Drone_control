#include "Barometer.h"

void Barometer::initialize(double *status)
{
    this->status = status;

    barometer.initialize();
}

void Barometer::update()
{
    barometer.refreshPressure();
    usleep(10000); // Waiting for pressure data ready
    barometer.readPressure();

    barometer.refreshTemperature();
    usleep(10000); // Waiting for temperature data ready
    barometer.readTemperature();

    barometer.calculatePressureAndTemperature();

    status[status_baro_pressure] = barometer.getPressure();
    status[status_baro_temperature] = barometer.getTemperature();
}
