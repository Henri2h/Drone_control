#include <Common/Util.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

#include <ctime>
#include <chrono>
#include <math.h>
#include <ncurses.h>

// references
#include "Ref.h"
#include "Data.cpp"

#include "FileManagement.cpp"


#include "RCInputManager.cpp"
#include "ServoManager.cpp"
#include "IMU.h"
#include "Stab/Stabilisation.cpp"
#include "LEDManager.cpp"
#include "Remote.h"
#include "SensorsManager/SensorManager.cpp"


using namespace std;

typedef std::chrono::high_resolution_clock TimeM;

int i = 0;
string version = "0.2.0";
double frequency = 900; // frequency


std::shared_ptr<LEDManager> led;
std::shared_ptr<RCInputManager> rc;
std::shared_ptr<ServoManager> servo;
std::shared_ptr<IMU> imu;
std::shared_ptr<Remote> remote;
std::shared_ptr<Stabilisation> stab;
std::shared_ptr<SensorManager> sMana;
static Data data;

auto time_start = TimeM::now();

auto last = TimeM::now();
auto t_last = TimeM::now();
float t = 0; // time

// status array definition : see Ref.h
// time
auto arming_t_started = TimeM::now();

// adding a status save system : ease dvpt
int f_display_iterationscount = 6000;