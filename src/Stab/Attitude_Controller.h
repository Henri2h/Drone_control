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
    float error[3] = {0, 0, 0};
    float Kp[3] = {0, 0, 0};
    float error_last[3] = {0, 0, 0};

    PID_Single_Axis loop_pos_pitch = PID_Single_Axis();
    PID_Single_Axis loop_speed_pitch = PID_Single_Axis();

    PID_Single_Axis loop_pos_roll = PID_Single_Axis();
    PID_Single_Axis loop_speed_roll = PID_Single_Axis();

    PID_Single_Axis loop_rate_yaw = PID_Single_Axis();
 // Angle mode
    // These parameters are very important for flight success
    // They must be tuned for each frame type, motors, and propeller used
    float anglePosPIDParams[4] = {0.010, 268, 0.5, 0.0}; // G, Kp, Kd, Ki
    float angleSpeedPIDParams[4] = {0.010, 192, 0.0, 0.0};

    // Accro mode
    // 450mm frame, 10x4.5" 2 blades propellers - Tested during flight test: OK
    float accroSpeedPIDParams[4] = {0.010, 192, 0.0, 0.0};

    // Yaw PID
    float yawSpeedPIDParams[4] = {0.010, 150.0, 0.0, 0.0}; // G, Kp, Kd, Ki
    
  public:
    Attitude_Controller();
    float cmd_last_pitch = 0;
    float cmd_last_roll = 0.0;
    float e_s = 0;

    // attention, pour permettre que les deux boucles soient indépendantes, il ne faut pas quelle fonctionnent à la même fréquence !! 
    void update(Data &data, float *cmd, float dt);
    void update_pid(float kp_atti, float kp_rate, float kd_rate);
};

#endif
