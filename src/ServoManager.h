/*
Provided to you by Emlid Ltd (c) 2015.
twitter.com/emlidtech || www.emlid.com || info@emlid.com

Example: Control servos connected to PWM driver onboard of Navio2 shield for Raspberry Pi.

Connect servo to Navio2's rc output and watch it work.
PWM_OUTPUT = 0 complies to channel number 1, 1 to channel number 2 and so on.
To use full range of your servo correct SERVO_MIN and SERVO_MAX according to it's specification.

To run this example navigate to the directory containing it and run following commands:
make
sudo ./Servo
*/

#ifndef ServoManager_H
#define ServoManager_H

#include <unistd.h>
#include "Navio2/PWM.h"
#include "Navio+/RCOutput_Navio.h"
#include "Navio2/RCOutput_Navio2.h"
#include "Common/Util.h"
#include <unistd.h>
#include <memory>

#include "Data.h"
#include "Ref.h"
#include "FileManagement.h"

#define SERVO_MIN 1000 //1250 /*mS*/
#define SERVO_MAX 1750 /*mS*/
#define MOTOR_COUNT 4

using namespace Navio;
using namespace std;

class ServoManager
{

    std::unique_ptr<RCOutput> get_rcout();
    std::unique_ptr<RCOutput> pwm = get_rcout();

public:
    ServoManager();
    int initialize();
    void setDuty(Data &data);
    void zero();
};

#endif
