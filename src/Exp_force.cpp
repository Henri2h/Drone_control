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

using namespace std;
int i = 0;
void setup(){
    printf("[ MAIN ] : Setup \n");

}

void loop(){
    i++;
    cout << i << "\n";
}

int main(int argc, char *argv[])
{
    printf("Starting ...\n");
    setup();
    while(true){
        loop();
    }
     
}

