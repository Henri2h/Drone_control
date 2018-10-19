/*
   omnibusf3.cpp : Board class for Omnibus F3

   Copyright (C) 2018 Simon D. Levy 

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

#pragma once

#include <boards/softquat.hpp>
#include <MPU6000.h>

class OmnibusF3 : public hf::SoftwareQuaternionBoard  {

    private:

        MPU6000 * _imu;

        void initMotors(void);
        void initUsb(void);
        void initImu(void);

    protected: 

        // Board class overrides
        virtual void     writeMotor(uint8_t index, float value) override;
        virtual void     delaySeconds(float sec) override;
        virtual void     setLed(bool is_on) override;
        virtual uint32_t getMicroseconds(void) override;
        virtual void     reboot(void) override;
        static void      outchar(char c);
        virtual uint8_t  serialAvailableBytes(void) override;
        virtual uint8_t  serialReadByte(void) override;
        virtual void     serialWriteByte(uint8_t c) override;

        // SoftwareQuaternionBoard class overrides
        virtual bool     imuRead(void) override;

    public:

        OmnibusF3(void);

}; // class OmnibusF3
