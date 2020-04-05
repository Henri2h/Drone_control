#ifndef GPS_H
#define GPS_H

#include <Common/Ublox.h>
#include <Common/Util.h>

#include "../Ref.h"

using namespace std;

class GPS
{
private:
	// create ublox class instance
	Ublox gps;
	bool ready;
	double *status;
	double previousStatus_gps_fix;

public:
	GPS();
    void initialize(double *status);

	void update();
};

#endif
