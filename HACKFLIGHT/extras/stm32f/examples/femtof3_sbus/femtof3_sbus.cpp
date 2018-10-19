/*
   Sketch for FemtoF3 Evo brushless board with SBUS receiver

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

#include <hackflight.hpp>
#include <mixers/quadx.hpp>
#include "femtof3.h"

constexpr uint8_t CHANNEL_MAP[6] = {0, 1, 2, 3, 4, 5};

static hf::Hackflight h;

extern "C" {

#include "../../common/sbusrx.h"

    void setup(void)
    {
        hf::Stabilizer * stabilizer = new hf::Stabilizer(
                1.00f, // Level P
                0.05f, // Gyro cyclic P
                0.00f, // Gyro cyclic I
                0.00f, // Gyro cyclic D
                0.10f, // Gyro yaw P
                0.01f, // Gyro yaw I
                8.58); // Demands to rate

        SBUS_Receiver * rc = new SBUS_Receiver(UARTDEV_3, CHANNEL_MAP);

        // XXX for some reason this has to be called both here and int Hackflight::init()
        rc->begin();

        // Initialize Hackflight firmware
        h.init(new FemtoF3(), rc, new hf::MixerQuadX(), stabilizer);
    }

    void loop(void)
    {
        h.update();
    }

} // extern "C"
