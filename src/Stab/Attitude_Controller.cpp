#include "Attitude_Controller.h"

Attitude_Controller::Attitude_Controller()
{
    loop_pos_pitch.setPID(0.01f, 250, 0, 0);
    loop_pos_roll.setPID(0.01f, 250, 0, 0); // according to librepilot
    loop_pos_yaw.setPID(0.01f, 250, 0, 0);
    // setup gains for the controllers (Kp)
}

// attention, pour permettre que les deux boucles soient indépendantes, il ne faut pas quelle fonctionnent à la même fréquence !!
void Attitude_Controller::update(Data &data, float *cmd, float dt)
{
    // pitch
    cmd[pid_pitch] = loop_pos_pitch.update(cmd[pid_pitch], data.ang[pid_pitch], dt);     // outer loop (attitude)
    cmd[pid_roll] = loop_pos_pitch.update(cmd[pid_roll], data.ang[pid_pitch], dt); //inner loop (rate)
    cmd[pid_yaw] = loop_pos_yaw.update(cmd[pid_yaw], data.ang[pid_yaw], dt);
}


void Attitude_Controller::update_pid(float *gains)
{
  loop_pos_pitch.setPID(1, gains[gains_pos_pitch_kp], gains[gains_pos_pitch_kd], gains[gains_pos_pitch_ki]);
  loop_pos_roll.setPID(1, gains[gains_pos_roll_kp], gains[gains_pos_roll_kd], gains[gains_pos_roll_ki]);
  loop_pos_yaw.setPID(1, gains[gains_pos_yaw_kp], gains[gains_pos_yaw_kd], gains[gains_pos_yaw_ki]);
}
