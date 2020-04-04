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

  // error
  float error[3] = {0, 0, 0};
  // Accro mode
  // 450mm frame, 10x4.5" 2 blades propellers - Tested during flight test: OK
  float accroSpeedPIDParams[4] = {0.010, 192, 0.0, 0.0};

  // Yaw PID
  float yawSpeedPIDParams[4] = {0.010, 150.0, 0.0, 0.0}; // G, Kp, Kd, Ki

public:
  Rate_Controller();
  void update(float *cmd, float *rates, float dt, float *pid_debug);
  void update_pid(float kp_rate, float kd_rate);
  void update_pid(float kp_rate, float kd_rate, float ki_rate);
  void update_pid(float *gains);
};

#endif