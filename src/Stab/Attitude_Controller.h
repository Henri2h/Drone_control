#ifndef Attitude_Controller_H
#define Attitude_Controller_H

#include <math.h>

#include "../Ref.h"
#include "PID_single_axis.h"
#include "../Data.h"

class Attitude_Controller
{
private:
  // error
  PID_Single_Axis loop_pos_pitch = PID_Single_Axis();
  PID_Single_Axis loop_pos_roll = PID_Single_Axis();
  PID_Single_Axis loop_pos_yaw = PID_Single_Axis();

public:
  Attitude_Controller();  // attention, pour permettre que les deux boucles soient indépendantes, il ne faut pas quelle fonctionnent à la même fréquence !!
  void update(Data &data, float *cmd, float dt);
  void update_pid(float *gains);
};

#endif
