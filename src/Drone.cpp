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
#include "PID.cpp"
#include "LEDManager.cpp"

using namespace std;
int i = 0;
string version = "0.0.4";

RCInputManager rc = RCInputManager();
ServoManager servo = ServoManager();
IMU imu = IMU();
IMU_data_processing imu_data_proc = IMU_data_processing();
PID pid = PID();
LEDManager led = LEDManager();

auto last = chrono::steady_clock::now();
auto dt_last = chrono::steady_clock::now();
float ang[3] = {0, 0, 0};

void setup()
{
    led.setKO();
    cout << "[ MAIN ] : " << version << "\n";
    printf("[ MAIN ] : Setup \n");
    servo.initialize();

    if (getuid())
    {
        printf("Not root. Please launch with root permission: sudo \n");
        throw "Not root";
    }
    led.setOK();
}

void loop()
{
    auto now = chrono::steady_clock::now();
    float diff_nano = chrono::duration<double, nano>(now - dt_last).count();
    dt_last = now;

    float dt = (diff_nano * pow(10, -9.0));
    int e = rc.read();
    auto imu_read = imu.read();
    imu_data_proc.getComplementar(ang, imu_read, dt);
    float *accPos = imu_data_proc.getAngleAccel(imu_read);

    cout << "Accelerometer : ";
    for (size_t i = 0; i < 3; i++)
    {
        cout << accPos[i] << ", ";
    }
    cout << "\n";

    cout << "gyr : ";
    for (size_t i = 0; i < 3; i++)
    {
        cout << ang[i] << ", ";
    }
    cout << "\n";

    servo.setDuty(e);
    i++;

    // loop control
    int r = 900;
    if (i % r == 0)
    {
        auto now = chrono::steady_clock::now();
        float diff = chrono::duration<double, nano>(now - last).count();

        float f = r / (diff * pow(10, -9.0));
        cout << "f : " << f << "Hz "
             << "\n";
        cout << i << " : " << e << "\n";
        last = now;
    }
}

int main(int argc, char *argv[])
{
    float frequency = 900; //Hz
    setup();

    // loop
    while (true)
    {
        loop();
    }
}
