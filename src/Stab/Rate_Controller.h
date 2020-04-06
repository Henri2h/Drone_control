#ifndef Rate_Controller_H
#define Rate_Controller_H

#include "../Ref.h"
#include "PID_single_axis.h"

class Rate_Controller
{
private:
  PID_Single_Axis loop_rate_pitch = PID_Single_Axis();
  PID_Single_Axis loop_rate_roll = PID_Single_Axis();
  PID_Single_Axis loop_rate_yaw = PID_Single_Axis();

public:
  Rate_Controller();
  void update(float *cmd, float *rates, float dt);
  void update_pid(float kp_rate, float kd_rate, float ki_rate);
  void update_pid(float *gains);
};

#endif