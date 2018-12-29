#include "../Ref.h"
#include "PID_single_axis.cpp"
// rate control
#include "Rate_Controller.cpp"
#include "Attitude_Controller.cpp"
#include "math.h"

class Stabilisation
{
  private:
    int acro_commands[3] = {0, 0, 0};
    int rate_max = 300;
    float cmd[4] = {0, 0, 0, 0};

    // controll loop
    Attitude_Controller attitude_c = Attitude_Controller();
    Rate_Controller rate_c = Rate_Controller();

    // Reception setup
    const float MAX_ANGLE = 55;      // (°) Max roll and pitch angles reachable in angle mode
    const float MAX_RATES = 220;     // (°/s) Max roll and pitch speed in accro mode
    const float MAX_YAW_SPEED = 220; // (°/s) Max yaw speed in accro and angle modes

    void getStabilisationMode(int *commands, int *stabilisation_mode)
    {
        int other_mode = 3;
        //3  rampe
        // 4 
        if (commands[cmd_flight_mode] > 1000)
        {
            if (*stabilisation_mode != other_mode)
            {
                std::cout << "[ STAB MODE ] : Angle"
                          << "\n";
                *stabilisation_mode = other_mode;
            }
        }
        else
        {
            if (*stabilisation_mode != 0)
            {
                std::cout << "[ STAB MODE ] : Rates"
                          << "\n";
                *stabilisation_mode = 0;
            }
        }
    }
    int iter = 0;
    int itermode = 0; // 0 croissant, 1 décroissant
  public:
  bool showGains = false;
    Stabilisation() {}
    int i = 0;
    int *Stabilize(int *stabilisation_mode, float *entree, int *motor_command, bool isArmed, int *commands, float *ang, float *rates, float dt, float *pid_debug)
    {
        iter++;
        i++;
        getStabilisationMode(commands, stabilisation_mode);

        // commands
        int cmd_raw[3] = {commands[cmd_pitch], commands[cmd_roll], commands[cmd_yaw]};

        // map values
        entree[3] = commands[cmd_throttle];
        cmd[pid_yaw] = mapValue(cmd_raw[pid_yaw], 1075, 1920, -MAX_YAW_SPEED, MAX_YAW_SPEED);

        // compute commands
        // update orders
        if (*stabilisation_mode == 1)
        {
            cmd[pid_pitch] = mapValue(cmd_raw[pid_pitch], 1075, 1920, -MAX_ANGLE, MAX_ANGLE);
            cmd[pid_roll] = mapValue(cmd_raw[pid_roll], 1075, 1920, -MAX_ANGLE, MAX_ANGLE);
            entree[pid_pitch] = cmd[pid_pitch];
            entree[pid_roll] = cmd[pid_roll];

            // fix values
            float kp_atti = mapValue(commands[cmd_kd], 922, 2077, 0, 3000);
            float kp_rate = mapValue(commands[cmd_kp], 922, 2077, 0, rate_max);
            float kd_rate = mapValue(commands[cmd_ki], 921, 2077, 0, 2);

            if (i % 600 == 0 && showGains)
            {
                i = 0;
                std::cout << "kp_atti : " << kp_atti << " kp_rate : " << kp_rate << " kd_rate : " << kd_rate << "\n";
            }

            attitude_c.update_pid(kp_atti, kp_rate, kd_rate);
            attitude_c.update(cmd, ang, rates, dt, pid_debug);
        }
        else
        {
            // param
            float a = 1 / 30.0;
            int max = 100;

            if (*stabilisation_mode == 0)
            {
                cmd[pid_pitch] = mapValue(cmd_raw[pid_pitch], 1075, 1920, -MAX_RATES, MAX_RATES);
                cmd[pid_roll] = mapValue(cmd_raw[pid_roll], 1075, 1920, -MAX_RATES, MAX_RATES);
            }
            else if (*stabilisation_mode == 2)
            { // essai indiciel
                commands[cmd_throttle] = 1550;

                cmd[pid_pitch] = 0;
                cmd[pid_roll] = max;

                entree[pid_roll] = max;
            }
            else if (*stabilisation_mode == 3)
            { // rampe
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
            }
            else if (*stabilisation_mode == 4)
            { // sinusoidal
                cmd[pid_pitch] = 0;
                cmd[pid_roll] = max * cos(a * iter / 120);
            }

            // fix values
            float kp_rate = mapValue(commands[cmd_kp], 922, 2077, 0, rate_max);
            float kd_rate = mapValue(commands[cmd_ki], 921, 2077, 0, 2);

            rate_c.update_pid(kp_rate, kd_rate);
            rate_c.update(cmd, rates, dt, pid_debug);
            if (i % 600 == 0 && showGains)
            {
                i = 0;
                std::cout << "kp_rate : " << kp_rate << " kd_rate : " << kd_rate << "\n";
                std::cout << "cmd : " << cmd[pid_roll] << " kd_rate : " << rates[pid_roll] << "\n";
            }
        }

        if (i % 100 == 0 and false)
        {
            i = 0;
            std::cout << "Cmd : " << cmd[pid_roll] << " " << rates[pid_roll] << "\n";
        }

        cmd[pid_pitch] = 0;
        cmd[pid_yaw] = 0;

        // motor commands
        /*
    self.esc.v0 = ui.throttle + pidRoll + pidPitch - pidYaw
    self.esc.v1 = ui.throttle - pidRoll + pidPitch + pidYaw
    self.esc.v2 = ui.throttle - pidRoll - pidPitch - pidYaw
    self.esc.v3 = ui.throttle + pidRoll - pidPitch + pidYaw
    */
        /*
        0     1
         \   /
          |D|
         /   \
        3     2
        // mixing system of ardupilot
        */
        if (isArmed && commands[cmd_throttle] > 1100)
        {
            motor_command[2] = commands[cmd_throttle] + cmd[pid_roll] - cmd[pid_pitch] - cmd[pid_yaw]; // motor 0
            motor_command[0] = commands[cmd_throttle] - cmd[pid_roll] - cmd[pid_pitch] + cmd[pid_yaw]; // motor 1
            motor_command[3] = 0;                                                                      //commands[cmd_throttle] - cmd[pid_roll] * mixing + cmd[pid_pitch] * mixing - cmd[pid_yaw] * mixing; // motor 2
            motor_command[1] = 0;                                                                      //commands[cmd_throttle] + cmd[pid_roll] * mixing + cmd[pid_pitch] * mixing + cmd[pid_yaw] * mixing; // motor 3
        }
        else
        {
            motor_command[0] = 0;
            motor_command[1] = 0;
            motor_command[2] = 0;
            motor_command[3] = 0;
            entree[0] = 0;
            entree[1] = 0;
            entree[2] = 0;
            entree[3] = 0;
        }
        return motor_command;
    }
};