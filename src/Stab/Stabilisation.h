#include "Rate_Controller.cpp"
#include "Attitude_Controller.cpp"
#include "math.h"
#include "../Data.h"
#include "../Ref.h"
#include "PID_single_axis.cpp"

class Stabilisation
{
private:
    int acro_commands[3];
    int pid_gains_rate_max;
    int pid_gains_attitude_max;
    float cmd[4];

    // controll loop
    Attitude_Controller attitude_c;
    Rate_Controller rate_c;

    // Reception setup
    static constexpr float MAX_ANGLE = 55;      // (°) Max roll and pitch angles reachable in angle mode
    static constexpr float MAX_RATES = 220;     // (°/s) Max roll and pitch speed in accro mode
    static constexpr float MAX_YAW_SPEED = 220; // (°/s) Max yaw speed in accro and angle modes

    void getValuesExperiments(Data &data, float dt);
    void getValuesStabRates(Data &data, float dt);
    void getValuesStabAttitude(Data &data, float dt);

    /// Change stabilisation depending on remote commands
    void getStabilisationMode(Data &data);

    int iter;
    int itermode; // 0 croissant, 1 décroissant
    double *status;
    int previous_cmd;

public:
    Stabilisation();
    bool showGains = false;
    void initialize(double *status);

    int i;
    int *Stabilize(Data &data, float dt);
}