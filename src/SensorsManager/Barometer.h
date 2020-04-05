#ifndef Barometer_H
#define Barometer_H

#include <Common/MS5611.h>
#include <Common/Util.h>
#include <unistd.h>
#include <stdio.h>

#include "../Ref.h"

class Barometer
{
    double *status;
    MS5611 barometer;

  public:
    void initialize(double *status);
    void update();
};

#endif
