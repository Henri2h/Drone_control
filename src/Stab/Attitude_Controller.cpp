#include "Attitude_Controller.h"

Attitude_Controller::Attitude_Controller()
{
    loop_pos_pitch.setPID(0.01f, 250, 0, 0);
    loop_pos_roll.setPID(0.01f, 250, 0, 0); // according to librepilot

    loop_speed_pitch.setPID(1, 0.006f, 0.000081f, 0.018f);
    loop_speed_roll.setPID(1, 0.0054f, 0.0081f, 0.016f);

    loop_rate_yaw.setPID(1, 0.00897f, 0.000076f, 0.01705f);
    // setup gains for the controllers (Kp)
}

// attention, pour permettre que les deux boucles soient indépendantes, il ne faut pas quelle fonctionnent à la même fréquence !!
void Attitude_Controller::update(Data &data, float *cmd, float dt)
{
    // pitch
    cmd[pid_pitch] = loop_pos_pitch.update(cmd[pid_pitch], data.ang[pid_pitch], dt);     // outer loop (attitude)
    cmd[pid_pitch] = loop_speed_pitch.update(cmd[pid_pitch], data.rates[pid_pitch], dt); //inner loop (rate)

    // roll
    float pid_cmd_roll = cmd[pid_roll];
    // angle
    pid_cmd_roll = loop_pos_roll.update(pid_cmd_roll, data.ang[pid_roll], dt); // outer loop pos
    e_s = e_s + pid_cmd_roll * dt;
    // feed it to the inner loop

    cmd[pid_roll] = loop_speed_roll.update(e_s, data.rates[pid_roll], dt, data.pid_debug); //inner loop speed
    // std::cout << "pid cmd  rool outer loop : " << pid_cmd_roll << " out cmd[] : " << cmd[pid_roll] << " " << "\n";

    // last
    cmd_last_roll = pid_cmd_roll;

    if (isnan(cmd[pid_roll]))
    {
        cmd[pid_roll] = 0;
        std::cout << "isnan\n";
    }

    cmd[pid_yaw] = loop_rate_yaw.update(cmd[pid_yaw], data.rates[pid_yaw], dt);
}

void Attitude_Controller::update_pid(float kp_atti, float kp_rate, float kd_rate)
{

    loop_pos_pitch.setPID(0.01, kp_rate, kd_rate, 0);
    loop_speed_pitch.setPID(0.01, kp_atti, 0, 0);

    loop_pos_roll.setPID(0.01, kp_rate, kd_rate, 0); // according to librepilot
    loop_speed_roll.setPID(0.01, kp_atti, 0, 0);
}
