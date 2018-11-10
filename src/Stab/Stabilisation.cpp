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

    int stabilisation_mode = -1;

    // controll loop
    Attitude_Controller attitude_c = Attitude_Controller();
    Rate_Controller rate_c = Rate_Controller();

    // Reception setup
    const float MAX_ANGLE = 45;      // (°) Max roll and pitch angles reachable in angle mode
    const float MAX_ROT_SPEED = 135; // (°/s) Max roll and pitch speed in accro mode
    const float MAX_YAW_SPEED = 135; // (°/s) Max yaw speed in accro and angle modes

  public:
    Stabilisation() {}

    int *Stabilize(int *motor_command, bool isArmed, int *commands, float *ang, float *rates, float dt)
    {

        // commands
        int cmd_raw[3] = {commands[cmd_roll], commands[cmd_pitch], commands[cmd_yaw]};

        float cmd[3] = {0, 0, 0}; // command mapped
        // map values
        cmd[pid_pitch] = mapValue(cmd_raw[pid_pitch], 1075, 1920, -MAX_ANGLE, MAX_ANGLE);
        cmd[pid_roll] = mapValue(cmd_raw[pid_roll], 1075, 1920, -MAX_ANGLE, MAX_ANGLE);
        cmd[pid_yaw] = mapValue(cmd_raw[pid_yaw], 1075, 1920, -MAX_YAW_SPEED, MAX_YAW_SPEED);

        // compute commands
        // update orders
        if (false)
        {
            attitude_c.update(cmd, ang, rates, dt);
        }
        else
        {
            rate_c.update(cmd, rates, dt);
        }
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
        float mixing = 0.5;
        if (isArmed && commands[cmd_throttle] > 1100)
        {
            motor_command[2] = commands[cmd_throttle] - cmd[pid_roll] * mixing + cmd[pid_pitch] * mixing - cmd[pid_yaw] * mixing; // motor 0
            motor_command[0] = commands[cmd_throttle] + cmd[pid_roll] * mixing + cmd[pid_pitch] * mixing + cmd[pid_yaw] * mixing; // motor 1
            motor_command[3] = commands[cmd_throttle] + cmd[pid_roll] * mixing - cmd[pid_pitch] * mixing - cmd[pid_yaw] * mixing; // motor 2
            motor_command[1] = commands[cmd_throttle] - cmd[pid_roll] * mixing - cmd[pid_pitch] * mixing + cmd[pid_yaw] * mixing; // motor 3
         
            motor_command[3] = 0;
            motor_command[1] = 0;
        }
        else
        {
            motor_command[0] = 0;
            motor_command[1] = 0;
            motor_command[2] = 0;
            motor_command[3] = 0;
        }
        return motor_command;
    }
};