#include "../Ref.h"

// rate control
#include "Rate_Controller.cpp"
#include "Attitude_Controller.cpp"

class Stabilisation
{
  private:
    int acro_commands[3] = {0, 0, 0};

    int stabilisation_mode = -1;

    // controll loop
    Attitude_Controller attitude_c = Attitude_Controller();
    Rate_Controller rate_c = Rate_Controller();

  public:
    Stabilisation() {}

    int *Stabilize(int *motor_command, bool isArmed, int *commands, float *ang, float *rates, float dt)
    {

        // commands
        int cmd_raw[3] = {commands[cmd_roll], commands[cmd_pitch], commands[cmd_yaw]};
        float cmd[3] = {0, 0, 0}; // command mapped
        // update orders
        attitude_c.update(cmd, ang, dt);
        rate_c.update(cmd, rates, dt);

        // motor commands
        /*
    self.esc.v0 = ui.throttle + pidRoll + pidPitch - pidYaw
    self.esc.v1 = ui.throttle - pidRoll + pidPitch + pidYaw
    self.esc.v2 = ui.throttle - pidRoll - pidPitch - pidYaw
    self.esc.v3 = ui.throttle + pidRoll - pidPitch + pidYaw
    */

        if (isArmed && commands[cmd_throttle] > 1100)
        {
            motor_command[0] = commands[cmd_throttle] - cmd[pid_roll] + cmd[pid_pitch] - cmd[pid_yaw];
            motor_command[1] = commands[cmd_throttle] + cmd[pid_roll] + cmd[pid_pitch] + cmd[pid_yaw];
            motor_command[2] = commands[cmd_throttle] + cmd[pid_roll] - cmd[pid_pitch] - cmd[pid_yaw];
            motor_command[3] = commands[cmd_throttle] - cmd[pid_roll] - cmd[pid_pitch] + cmd[pid_yaw];
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