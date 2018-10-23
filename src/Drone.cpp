/*
Provided to you by Emlid Ltd (c) 2015.
twitter.com/emlidtech || www.emlid.com || info@emlid.com

Example: Control servos connected to PWM driver onboard of Navio2 shield for Raspberry Pi.

Connect servo to Navio2's rc pid_out and watch it work.
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
#include "Remote.cpp"

using namespace std;

typedef std::chrono::high_resolution_clock TimeM;

int i = 0;
string version = "0.0.5";

RCInputManager rc = RCInputManager();
ServoManager servo = ServoManager();
IMU imu = IMU();
IMU_data_processing imu_data_proc = IMU_data_processing();
PID pid = PID();
LEDManager led = LEDManager();
Remote remote = Remote();

auto time_start = TimeM::now();

auto last = TimeM::now();
auto dt_last = TimeM::now();
float t = 0; // time
float ang[3] = {0, 0, 0};
float motors[4] = {0, 0, 0, 0};
int commands[5] = {0, 0, 0, 0};
int pid_out[3] = {0, 0, 0};
int motors_output[4] = {0, 0, 0, 0};

void setup()
{
    led.setKO();

    // check if root
    if (getuid())
    {
        printf("Not root. Please launch with root permission: sudo \n");
        throw "Not root";
    }

    if (check_apm())
    {
        cout << "APM is running ... could not run";
        throw "APM running";
    }

    cout << "[ MAIN ] : " << version << "\n";
    printf("[ MAIN ] : Setup \n");
    servo.initialize();

    // launch remote

    remote.launch(ang, &t);

    led.setOK();
}

void loop()
{
    auto now = TimeM::now();
    double now_n = chrono::duration<double, nano>(now - time_start).count();
    t = now_n * pow(10, -9.0);

    double diff_nano = chrono::duration<double, nano>(now - dt_last).count();
    dt_last = now;

    float dt = (diff_nano * pow(10, -9.0));
    rc.read(commands);
    auto imu_read = imu.read();
    imu_data_proc.getComplementar(ang, imu_read, dt);
    float *accPos = imu_data_proc.getAngleAccel(imu_read);

    if (false)
    {
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
    }
    // pid
    pid.getPID(pid_out, commands, ang, t);
    // motor commands
    /*
    self.esc.v0 = ui.throttle + pidRoll + pidPitch - pidYaw
    self.esc.v1 = ui.throttle - pidRoll + pidPitch + pidYaw
    self.esc.v2 = ui.throttle - pidRoll - pidPitch - pidYaw
    self.esc.v3 = ui.throttle + pidRoll - pidPitch + pidYaw
    */

    motors_output[0] = commands[3] + pid_out[0] + pid_out[1] - pid_out[2];
    motors_output[1] = commands[3] - pid_out[0] + pid_out[1] + pid_out[2];
    motors_output[2] = commands[3] - pid_out[0] - pid_out[1] - pid_out[2];
    motors_output[3] = commands[3] + pid_out[0] - pid_out[1] + pid_out[2];

    servo.setDuty(motors_output);
    i++;

    // display frequency
    int r = 900;
    if (i % r == 0)
    {

        auto now = TimeM::now();
        float diff = chrono::duration<double, nano>(now - last).count();

        float f = r / (diff * pow(10, -9.0));
        cout << "f : " << f << "Hz "
             << "\n";
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
