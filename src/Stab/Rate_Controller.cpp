#include "Rate_Controller.h"

Rate_Controller::Rate_Controller()
{
  // setup gains for the controllers
  loop_rate_pitch.setPID(0.01, 192, 0, 0);
  loop_rate_roll.setPID(0.01, 192, 0, 0);
  loop_rate_yaw.setPID(0.01, 150, 0, 0);
}

void Rate_Controller::update(float *cmd, float *rates, float dt, float *pid_debug)
{
  // inner loop
  cmd[pid_pitch] = loop_rate_pitch.update(cmd[pid_pitch], rates[pid_pitch], dt);
  cmd[pid_roll] = loop_rate_roll.update(cmd[pid_roll], rates[pid_roll], dt, pid_debug);
  //cout << "out : " << cmd[pid_roll] << " rates : " << rates[pid_roll] << "\n";
  cmd[pid_yaw] = loop_rate_yaw.update(cmd[pid_yaw], rates[pid_yaw], dt);
}

void Rate_Controller::update_pid(float kp_rate, float kd_rate)
{
  loop_rate_pitch.setPID(1, kp_rate, kd_rate, 0);
  loop_rate_roll.setPID(1, kp_rate, kd_rate, 0);
}

void Rate_Controller::update_pid(float kp_rate, float kd_rate, float ki_rate)
{
  loop_rate_pitch.setPID(1, kp_rate, kd_rate, ki_rate);
  loop_rate_roll.setPID(1, kp_rate, kd_rate, ki_rate);
}

void Rate_Controller::update_pid(float *gains)
{
  loop_rate_pitch.setPID(1, gains[gains_rates_pitch_kp], gains[gains_rates_pitch_kd], gains[gains_rates_pitch_ki]);
  loop_rate_roll.setPID(1, gains[gains_rates_roll_kp], gains[gains_rates_roll_kd], gains[gains_rates_roll_ki]);
  loop_rate_yaw.setPID(1, gains[gains_rates_yaw_kp], gains[gains_rates_yaw_kd], gains[gains_rates_yaw_ki]);
}
