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
#include <Common/Util.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <string>

#include <ctime>
#include <chrono>
#include <math.h> 


#include "RCInputManager.cpp"
#include "ServoManager.cpp"
#include "IMU.cpp"
#include "IMU_data_processing.cpp"

using namespace std;
int i = 0;
string version = "beta";

RCInputManager rc = RCInputManager();
ServoManager servo = ServoManager();
IMU imu = IMU();

auto last = chrono::steady_clock::now();

void setup(){
    cout << "[ MAIN ] : "  << version << "\n";
    printf("[ MAIN ] : Setup \n");
    servo.initialize();
}

void loop(){
    int e = rc.read();
    imu.read();
    servo.setDuty(e);
    i++;
    
    // loop control
    int r = 900;
    if(i%r == 0){
        auto now = chrono::steady_clock::now();
        float diff = chrono::duration <double, nano> (now - last).count();

        float f = r / (diff * pow(10, -9.0))  ;
        cout << "f : " << f << "Hz " << diff * pow(10, -9.0) << " ns \n";
        cout << i << "," << e << "\n";
        last = now;
    }
}

int main(int argc, char *argv[])
{
    printf("Starting ...\n");
    setup();
    while(true){
        loop();
    }
     
}

