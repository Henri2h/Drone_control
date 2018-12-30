/*

Drone control

*/

#include <Common/Util.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

#include <ctime>
#include <chrono>
#include <math.h>


#include "RCInputManager.cpp"
#include "ServoManager.cpp"
#include "IMU.cpp"
#include "Stab/Stabilisation.cpp"
#include "LEDManager.cpp"
#include "Remote.cpp"

#include "Ref.h"

#define LOGNAME_FORMAT "log/data_%Y%m%d_%H%M%S.csv"
#define LOGNAME_SIZE 32

using namespace std;

typedef std::chrono::high_resolution_clock TimeM;

int i = 0;
string version = "0.1.7";

RCInputManager rc = RCInputManager();
ServoManager servo = ServoManager();
IMU imu = IMU();
LEDManager led = LEDManager();
Remote remote = Remote();
Stabilisation stab = Stabilisation();

auto time_start = TimeM::now();

auto last = TimeM::now();
auto t_last = TimeM::now();
float t = 0; // time
float ang[3] = {0, 0, 0};
float ang_acc[3] = {0, 0, 0};

float acceleration[3] = {0, 0, 0};
float rates[3] = {0, 0, 0};

int commands[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
float commands_gen[4] = {0, 0, 0, 0}; // to have an idea of what commands we send to the control algorithm, in ° or in °/s

// remote
int pid_out[3] = {0, 0, 0};
float pid_debug[3] = {0, 0, 0};

float motors[4] = {0, 0, 0, 0};
int motors_output[4] = {0, 0, 0, 0};
int sensors[2] = {0, 0}; // pressure, temperature
int frequency_crtl[] = {0, 0};

// safety
bool isArmed = false;
bool isArming = false;
int stabilisation_mode = 0; // rates = 0, angle = 1
    //float cmd[3] = {0, 0, 0};   // command mapped

// saving

bool runSaving = false;
int orders [10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float status [10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
/* status array definition
0 :
1 :

// GPS 
2 : Latitude
3 : Longitude
4 : Heigh

// Pressure and temperature
5 : Pressure
6 : Temperature
*/
// time
auto arming_t_started = TimeM::now();

// adding a status save system : ease dvpt
void updateStatus(){
    if(runSaving)
    {  status[0] = 1;
    }
    else{
        status[0] = 0;
    }
    
    }

void updateOrders(){
    if(orders[0] == 1){runSaving = true;}
    else {runSaving = false;}

    if(orders[1] == 1){stab.showGains = true;}
    else{stab.showGains = false;}
   }


void listFiles(){}
void safety()
{
    //cout << "throttle : " << commands[cmd_throttle] << " arming : " << commands[cmd_arming] <<  "\n";
    if (commands[cmd_throttle] < 1100) // evaluation must be done at 0 throttle !!!!
    {
        if (commands[cmd_arming] > 1500)
        {
            double t_diff = chrono::duration<double, nano>(t_last - arming_t_started).count();

            if (isArming == false) // is arming
            {
                cout << "ARMING\n";
                isArming = true;
                arming_t_started = TimeM::now();
                led.setArming();
            }
            else if (isArming && t_diff * pow(10, -9) > 2 && isArmed == false)
            {
                cout << "ARMED\n";
                isArmed = true;
                led.setArmed();
            }
        }
        else
        {
            isArmed = false;
            isArming = false;
            led.setOK();
        }
    }
}

void setup()
{
    led.setKO();

    // check if root
    if (getuid())
    {
        printf("Not root. Please launch with root permission: sudo \n");
        throw "Not root";
    }

    if (check_apm()) // check if apm (ardupilot) is running
    {
        cout << "APM is running ... could not run";
        throw "APM running";
    }

    cout << "[ MAIN ] : " << version << "\n";
    printf("[ MAIN ] : Setup \n");
    servo.initialize();

    // launch remote

    remote.launch(commands_gen, ang, acceleration, rates, pid_out, pid_debug, sensors, status, orders, &t);

    led.setOK();
}

void loop()
{
    // get informations
    auto now = TimeM::now();
    imu.update(); // get imu data just after timing

    rc.read(commands);

    // process informations
    double now_n = chrono::duration<double, nano>(now - time_start).count();
    double diff_nano = chrono::duration<double, nano>(now - t_last).count();

    t_last = now;

    // dt : convert nano to seconds
    float dt = diff_nano * pow(10, -9.0);
    t = now_n * pow(10, -9.0);

    // update imu
    imu.setDt(dt); // update dt

    // safety :
    safety();

    // angles
    imu.getComplementar(ang, ang_acc); // get angles
    imu.getRates(rates);
    imu.getAcceleration(acceleration);

    // stabilize
    stab.Stabilize(&stabilisation_mode, commands_gen, motors_output, isArmed, commands, ang, rates, dt, pid_debug);
    servo.setDuty(motors_output);
    i++;

    // led display
    led.update();

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

        //        pid.displayK();
    }
}

int isFileOpen = false;
ofstream myfile;

void saveData(bool save)
{
    string sep = ",";
    if (save && isFileOpen == false)
    {
        std::cout << "[ FileSaving ] : going to save\n";
        // open file
        static char name[LOGNAME_SIZE];
        time_t now = time(0);
        strftime(name, sizeof(name), LOGNAME_FORMAT, localtime(&now));
        myfile.open(name);

        isFileOpen = true;

        myfile << "t" << sep
               << "stabilisation mode" << sep
               << "commands_gen[0]" << sep << "commands_gen[1]" << sep << "commands_gen[2]" << sep << "commands_gen[3]" << sep
               << "rates[0]" << sep << "rates[1]" << sep << "rates[2]" << sep
               << "acceleration[0]" << sep << "acceleration[1]" << sep << "acceleration[2]" << sep
               << "ang[0]" << sep << "ang[1]" << sep << "ang[2]" << sep
               << "pid_out[0]" << sep << "pid_out[1]" << sep << "pid_out[2]"
               << "\n";

               
    }

    if(save){
    // save data in a csv format
    myfile << t << sep
           << stabilisation_mode << sep
           << commands_gen[0] << sep << commands_gen[1] << sep << commands_gen[2] << sep << commands_gen[3] << sep
           << rates[0] << sep << rates[1] << sep << rates[2] << sep
           << acceleration[0] << sep << acceleration[1] << sep << acceleration[2] << sep
           << ang[0] << sep << ang[1] << sep << ang[2] << sep
           << pid_debug[0] << sep << pid_debug[1] << sep << pid_debug[2]
           << "\n";
    }

    if (save == false && isFileOpen)
    {
        std::cout << "[ FileSaving ] : file saving ended\n";
        isFileOpen = false;
        myfile.close();
    }
}



auto last_call = TimeM::now();
bool useTimer = true;
int main(int argc, char *argv[])
{
    double frequency = 900;      //Hz
    double f_dt = 1 / frequency; //seconds
    long long int f_dt_n = f_dt * pow(10, 9);
    cout << "[ MAIN ] : frequency : " << f_dt_n << "Hz \n";
    setup();

    // timer variables
    double next_call = 0;
    last_call = TimeM::now(); // reset timmer
    // loop
    while (true)
    {
        // waiting
        if (useTimer)
        {
            last_call += chrono::nanoseconds(f_dt_n);
            while (pow(10, -9) * chrono::duration<double, nano>(last_call - TimeM::now()).count() > 0)
            {
                // micro second
                // if error is superior to 10 micro second, reset clock
                if (pow(10, -3) * chrono::duration<double, nano>(TimeM::now() - last_call).count() > 10)
                {
                    cout << "Clock reset\n";
                    last_call = TimeM::now();
                }
            }
        }

        // wait that we are at beggining

        // call loop
        loop();
        updateOrders();
        updateStatus();
        
        saveData(runSaving);
        
    }
}
