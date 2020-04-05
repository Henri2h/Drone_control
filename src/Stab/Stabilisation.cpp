
// rate control
#include "Stabilisation.h"

Stabilisation::Stabilisation()
{
    pid_gains_rate_max = 300;
    pid_gains_attitude_max = 3000;

    // controll loop
    attitude_c = Attitude_Controller();
    rate_c = Rate_Controller();
    iter = 0;
    itermode = 0; // 0 croissant, 1 décroissant
    previous_cmd = 0;
}

void Stabilisation::getValuesExperiments(Data &data, float dt)
{
    // if we are in experiment mode
    // param
    float a = 1 / 40.0;
    int max = 100;

    if (data.stabilisation_mode == 2)
    { // essai indiciel
        data.commands[cmd_throttle] = 1550;

        cmd[pid_pitch] = 0;
        cmd[pid_roll] = max;

        data.entree[pid_roll] = max;
    }
    else if (data.stabilisation_mode == 3)
    { // rampe
        data.commands[cmd_throttle] = 1550;
        cmd[pid_pitch] = 0;

        if (itermode == 0)
        {
            cmd[pid_roll] = a * iter;
            if (iter * a > max)
            {
                itermode = 1;
                iter = 0;
            }
        }
        else
        {
            cmd[pid_roll] = max - a * iter;
            if (max - a * iter <= 0)
            {
                itermode = 0;
                iter = 0;
            }
        }

        data.entree[pid_roll] = cmd[pid_roll];
    }
    else if (data.stabilisation_mode == 4)
    {
        // sinusoidal
        data.commands[cmd_throttle] = 1550;
        cmd[pid_pitch] = 0;
        cmd[pid_roll] = max * cos(a * iter / 120);
        data.entree[pid_roll] = cmd[pid_roll];
    }
}

void Stabilisation::getValuesStabRates(Data &data, float dt)
{
    // map kd
    double kp_rate = utils::mapValue(data.commands[cmd_kp], 922, 2077, 0, 1);
    double kd_rate = utils::mapValue(data.commands[cmd_kd], 922, 2077, 0, 1);
    double ki_rate = utils::mapValue(data.commands[cmd_ki], 921, 2077, 0, 1);

    data.status[status_gains_rate_kp] = kp_rate;
    data.status[status_gains_rate_kd] = kd_rate;
    data.status[status_gains_rate_ki] = ki_rate;

    rate_c.update_pid(kp_rate, kd_rate, ki_rate);
    rate_c.update(cmd, data.rates, dt, data.pid_debug);
}

void Stabilisation::getValuesStabAttitude(Data &data, float dt)
{
    data.entree[pid_pitch] = cmd[pid_pitch];
    data.entree[pid_roll] = cmd[pid_roll];

    // fix values
    float kp_atti = utils::mapValue(data.commands[cmd_kd], 922, 2077, 0, pid_gains_attitude_max);
    float kp_rate = utils::mapValue(data.commands[cmd_kp], 922, 2077, 0, pid_gains_rate_max);
    float kd_rate = utils::mapValue(data.commands[cmd_ki], 921, 2077, 0, 2);

    if (i % 600 == 0 && showGains)
    {
        i = 0;
        std::cout << "kp_atti : " << kp_atti << " kp_rate : " << kp_rate << " kd_rate : " << kd_rate << "\n";
    }

    data.status[status_gains_atti_kp] = kp_atti;
    data.status[status_gains_rate_kp] = kp_rate;
    data.status[status_gains_rate_kd] = kd_rate;

    attitude_c.update_pid(kp_atti, kp_rate, kd_rate); // update gains
    attitude_c.update(data, cmd, dt);
}

/// Change stabilisation depending on remote commands
void Stabilisation::getStabilisationMode(Data &data)
{
    int other_mode = 4;

    // 2 : indiciel
    // 3 : rampe
    // 4 : sinusoidal

    if (data.stabilisation_mode != 0 and (cmd_mode_1 - cmd_mode_delta) < data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode] < (cmd_mode_1 + cmd_mode_delta))
    {
        std::cout << "[ STAB MODE ] : 0 : Rates mode"
                  << "\n";
        data.stabilisation_mode = 0;
    }
    else if (data.stabilisation_mode != 1 and (cmd_mode_2 - cmd_mode_delta) < data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode] < (cmd_mode_2 + cmd_mode_delta))
    {
        std::cout << "[ STAB MODE ] : 1 : Angle mode"
                  << "\n";
        data.stabilisation_mode = 1;
    }
    else if (data.stabilisation_mode != 5 and (cmd_mode_3 - cmd_mode_delta) < data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode] < (cmd_mode_3 + cmd_mode_delta))
    {
        std::cout << "[ STAB MODE ] : 5 direct control"
                  << "\n";
        data.stabilisation_mode = 5;
    }
    else if (data.stabilisation_mode != 2 and (cmd_mode_4 - cmd_mode_delta) < data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode] < (cmd_mode_4 + cmd_mode_delta))
    {
        std::cout << "[ STAB MODE ] : 2 : Essai indiciel"
                  << "\n";
        data.stabilisation_mode = 2;
    }
    else if (data.stabilisation_mode != 3 and (cmd_mode_5 - cmd_mode_delta) < data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode] < (cmd_mode_5 + cmd_mode_delta))
    {
        std::cout << "[ STAB MODE ] : 3: Essai rampe"
                  << "\n";
        data.stabilisation_mode = 3;
    }
    if (data.stabilisation_mode != 4 and (cmd_mode_6 - cmd_mode_delta) < data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode] < (cmd_mode_6 + cmd_mode_delta))
    {
        std::cout << "[ STAB MODE ] : 4 : Essai sinusoidal"
                  << "\n";
        data.stabilisation_mode = 4;
    }

    // now we can update the status
    data.status[status_stab_mode] = data.stabilisation_mode;
}

void Stabilisation::initialize(double *status)
{
    this->status = status;
}

void Stabilisation::printGains(int *cmd, int kp_rate, int kd_rate, Data &data)
{

    std::cout << "kp_rate : " << kp_rate << " kd_rate : " << kd_rate << "\n";
    std::cout << "cmd : " << cmd[pid_roll] << " kd_rate : " << data.rates[pid_roll] << "\n";
}

void Stabilisation::printCommands(int *cmd, Data &data)
{
    std::cout << "Cmd : " << cmd[pid_roll] << " " << data.rates[pid_roll] << "\n";
}

// main stabilise function

int *Stabilisation::Stabilize(Data &data, float dt)
{
    iter++;
    i++;

    // going to get stabilisation mode
    getStabilisationMode(data);

    // map values
    data.entree[entree_throttle] = data.commands[cmd_throttle];

    if (status[status_experience_mode] != 2)
    // we are not doing an experiment...
    // real flight controler
    {
        // map values :
        float MAX = MAX_ANGLE;
        if (data.stabilisation_mode == 0)
        { // rates mode
            MAX = MAX_RATES;
        }

        int cmd_raw[3] = {data.commands[cmd_pitch], data.commands[cmd_roll], 2000 - data.commands[cmd_yaw]};

        cmd[pid_pitch] = utils::mapValue(cmd_raw[pid_pitch], 1075, 1920, -MAX, MAX);
        cmd[pid_roll] = utils::mapValue(cmd_raw[pid_roll], 1075, 1920, -MAX, MAX);
        cmd[pid_yaw] = utils::mapValue(cmd_raw[pid_yaw], 1075, 1920, -MAX_YAW_SPEED, MAX_YAW_SPEED);

        // compute commands
        // update orders
        if (data.stabilisation_mode == 1) // attitude mode
        {
            getValuesStabAttitude(data, dt);
        }
        else // rate mode
        {
            getValuesStabRates(data, dt);
        }
    }

    else if (status[status_experience_mode])
    /*
    Experience mode : we take the control from the user and perform a standardized experiment
    */
    {
        if (data.time_exp > exp_time_motor_start && data.time_exp < exp_time_motor_stop)
        {
            data.commands[cmd_throttle] = 1550;
            data.commands_gen[entree_throttle] = 1400;

            cmd[pid_roll] = 0;

            // cmd
            if (data.time_exp > exp_time_cmd_start && data.time_exp < exp_time_cmd_stop)
            {

                int cmd_exp = 100;
                data.commands_gen[entree_roll] = cmd_exp;
                cmd[pid_roll] = cmd_exp;
            }

            float kp = data.controller_gains[gains_rates_pitch_kp];
            data.pid_debug[0] = kp;
            rate_c.update_pid(kp, 0, 0);
            rate_c.update(cmd, data.rates, dt, data.pid_debug);

            data.isArmed = true;
            // rates
        }

        cmd[pid_pitch] = 0;
        cmd[pid_yaw] = 0;
    }
    else
    {
        std::cout << "Error : No stabilisation mode\n";
    }

    // motor commands
    /*
    self.esc.v0 = ui.throttle + pidRoll + pidPitch - pidYaw
    self.esc.v1 = ui.throttle - pidRoll + pidPitch + pidYaw
    self.esc.v2 = ui.throttle - pidRoll - pidPitch - pidYaw
    self.esc.v3 = ui.throttle + pidRoll - pidPitch + pidYaw
    */

    // we can now send commands to the motors
    if (data.isArmed && data.commands[cmd_throttle] > 1100)
    {

        /* : miximg system of the ardupilot flight control
        2     0
         \   /
          |D|
         /   \
        1     3
        */
        data.motors_output[2] = data.commands[cmd_throttle] + cmd[pid_roll] + cmd[pid_pitch] + cmd[pid_yaw]; // motor 0
        data.motors_output[0] = data.commands[cmd_throttle] - cmd[pid_roll] + cmd[pid_pitch] - cmd[pid_yaw]; // motor 1
        data.motors_output[3] = data.commands[cmd_throttle] - cmd[pid_roll] - cmd[pid_pitch] + cmd[pid_yaw];
        data.motors_output[1] = data.commands[cmd_throttle] + cmd[pid_roll] - cmd[pid_pitch] - cmd[pid_yaw];
        // motor 2
        // std::cout << "danger : " << data.motors_output[0] << "\n";
    }

    // DANGER : mode 5 gives direct control to the motor 3
    else if (data.stabilisation_mode == 5)
    {
        if (data.commands[cmd_arming] > 1400)
        {
            data.motors_output[3] = previous_cmd;
            cmd[pid_roll] = previous_cmd;
        }
        else
        {
            cmd[pid_roll] = 0;
            data.motors_output[3] = 0;
            previous_cmd = data.commands[cmd_throttle];

            if (i % 40 == 0)
            {

                i = 0;
                std::cout << previous_cmd << "\n";
            }
        }
    }

    else
    {
        data.motors_output[0] = 0;
        data.motors_output[1] = 0;
        data.motors_output[2] = 0;
        data.motors_output[3] = 0;
        data.entree[0] = 0;
        data.entree[1] = 0;
        data.entree[2] = 0;
        data.entree[3] = 0;
    }
}
