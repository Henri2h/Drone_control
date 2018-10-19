/*
   DSMX_VL53L1X.ino : Hackflight sketch for Ladybug Flight Controller with Spektrum DSMX receiver and
                      VL53L1X distance sensor

   Copyright (c) 2018 Simon D. Levy

   This file is part of Hackflight.

   Hackflight is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Hackflight is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with Hackflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <VL53L1X.h>

#include "hackflight.hpp"
#include "boards/ladybug.hpp"
#include "receivers/dsmx.hpp"
#include "sensors/rangefinder.hpp"
#include "pidcontrollers/loiter.hpp"
#include "mixers/quadx.hpp"

constexpr uint8_t CHANNEL_MAP[6] = {0, 1, 2, 3, 6, 4};

hf::Hackflight h;

hf::DSMX_Receiver rc = hf::DSMX_Receiver(CHANNEL_MAP);

hf::MixerQuadX mixer;

hf::Stabilizer stabilizer = hf::Stabilizer(
        0.20f,      // Level P
        0.225f,     // Gyro cyclic P
        0.001875f,  // Gyro cyclic I
        0.375f,     // Gyro cyclic D
        1.0625f,    // Gyro yaw P
        0.005625f); // Gyro yaw I

hf::Loiter loiter = hf::Loiter(
	0.40f,  // Altitude P
    0.2f,   // Altitude D
    0.f,    // Cyclic P
    1.0f,   // throttleScale
    0.04f); // minAltitude


class VL53L1X_Rangefinder : public hf::Rangefinder {

    private:

        VL53L1X _distanceSensor;

    protected:

        virtual bool distanceAvailable(float & distance) override
        {
            if (_distanceSensor.newDataReady()) {
                distance = _distanceSensor.getDistance() / 1000.f; // mm => m
                return true;
            }
            return false;
        }

    public:

        void begin(void)
        {
            _distanceSensor.begin();
        }

};

VL53L1X_Rangefinder rangefinder;

void setup(void)
{
    // Add some "software trim" to the receiver
    rc.setTrimPitch(-.1);

    // Initialize Hackflight firmware
    // We're using an older ladybug with LED on pin A1
    h.init(new hf::Ladybug(A1), &rc, &mixer, &stabilizer);

    // Add rangefinder sensor
    rangefinder.begin();
    h.addSensor(&rangefinder);

    // Add Loiter PID controller for aux switch position 2
    h.addPidController(&loiter, 2);
}

void loop(void)
{
    h.update();
}
