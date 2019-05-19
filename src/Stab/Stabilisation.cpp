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
    int pid_gains_rate_max = 300;
    int pid_gains_attitude_max = 3000   ;
    float cmd[4] = {0, 0, 0, 0};

    // controll loop
    Attitude_Controller attitude_c = Attitude_Controller();
    Rate_Controller rate_c = Rate_Controller();

    // Reception setup
    const float MAX_ANGLE = 55;      // (°) Max roll and pitch angles reachable in angle mode
    const float MAX_RATES = 220;     // (°/s) Max roll and pitch speed in accro mode
    const float MAX_YAW_SPEED = 220; // (°/s) Max yaw speed in accro and angle modes

    /// Change stabilisation depending on remote commands
    void getStabilisationMode(Data data, int data.stabilisation_mode) 
    {
        int other_mode = 4;
        
        // 2 : indiciel
        // 3 : rampe
        // 4 : sinusoidal
        
        if (data.stabilisation_mode != 0 and (cmd_mode_1 - cmd_mode_delta) <  data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode]  < (cmd_mode_1 + cmd_mode_delta))
        {
                std::cout << "[ STAB MODE ] : 1 : Rates mode" << "\n";
                data.stabilisation_mode = 0;   
        }
        else if (data.stabilisation_mode != 1 and (cmd_mode_2 - cmd_mode_delta) <  data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode]  < (cmd_mode_2 + cmd_mode_delta) )
        {
                std::cout << "[ STAB MODE ] : 2 : Angle mode" << "\n";
                data.stabilisation_mode = 1;   
        }
        else if (data.stabilisation_mode != 5 and (cmd_mode_3 - cmd_mode_delta) <  data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode]  < (cmd_mode_3 + cmd_mode_delta) )
        {
                std::cout << "[ STAB MODE ] : 5 direct control" << "\n";
                data.stabilisation_mode = 5;   
        }
        else if (data.stabilisation_mode != 2 and (cmd_mode_4 - cmd_mode_delta) <  data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode]  < (cmd_mode_4 + cmd_mode_delta) )
        {
                std::cout << "[ STAB MODE ] : 4 : Essai indiciel" << "\n";
                data.stabilisation_mode = 2;   
        }
        else if (data.stabilisation_mode != 3 and (cmd_mode_5 - cmd_mode_delta) <  data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode]  < (cmd_mode_5 + cmd_mode_delta) )
        {
                std::cout << "[ STAB MODE ] : 5 : Essai rampe" << "\n";
                data.stabilisation_mode = 3;   
        }
        if (data.stabilisation_mode != 4 and (cmd_mode_6 - cmd_mode_delta) <  data.commands[cmd_flight_mode] and data.commands[cmd_flight_mode]  < (cmd_mode_6 + cmd_mode_delta) )
        {
                std::cout << "[ STAB MODE ] : 6 : Essai sinusoidal" << "\n";
                data.stabilisation_mode = 4;   
        }
        
        
    }

    int iter = 0;
    int itermode = 0; // 0 croissant, 1 décroissant
    double * status;
    int previous_cmd = 0;



  public:
    
    bool showGains = false;
    void initialize(double * status) {
        this->status = status;
    }
    
    int i = 0;
    int *Stabilize(Data data, float dt)
    {
        iter++;
        i++;
        getStabilisationMode(data);
        data.status[status_stab_mode] = data.stabilisation_mode;

        // commands
        int cmd_raw[3] = {data.commands[cmd_pitch], data.commands[cmd_roll], data.commands[cmd_yaw]};

        // map values
        entree[3] = data.commands[cmd_throttle];



        // map values : 
        float MAX = MAX_ANGLE;
        if(data.stabilisation_mode == 0){ // rates mode
            MAX = MAX_RATES;
        }

        cmd[pid_pitch] = mapValue(cmd_raw[pid_pitch], 1075, 1920, -MAX, MAX);
        cmd[pid_roll] = mapValue(cmd_raw[pid_roll], 1075, 1920, -MAX, MAX);
        cmd[pid_yaw] = mapValue(cmd_raw[pid_yaw], 1075, 1920, -MAX_YAW_SPEED, MAX_YAW_SPEED);
        


        // compute commands
        // update orders
        if (data.stabilisation_mode == 1)
        {
            entree[pid_pitch] = cmd[pid_pitch];
            entree[pid_roll] = cmd[pid_roll];

            // fix values
            float kp_atti = mapValue(commands[cmd_kd], 922, 2077, 0, pid_gains_attitude_max);
            float kp_rate = mapValue(commands[cmd_kp], 922, 2077, 0, pid_gains_rate_max);
            float kd_rate = mapValue(commands[cmd_ki], 921, 2077, 0, 2);

            if (i % 600 == 0 && showGains)
            {
                i = 0;
                std::cout << "kp_atti : " << kp_atti << " kp_rate : " << kp_rate << " kd_rate : " << kd_rate << "\n";
            }

            status[status_gains_atti_kp] = kp_atti;
            status[status_gains_rate_kp] = kp_rate;
            status[status_gains_rate_kd] = kd_rate;

            attitude_c.update_pid(kp_atti, kp_rate, kd_rate);// update gains
            attitude_c.update(data, cmd, dt);
        }

        else // rate mode
        {
            // param
            float a = 1 / 40.0;
            int max = 100;

           if (data.stabilisation_mode == 2)
            { // essai indiciel
                data.commands[cmd_throttle] = 1550;

                cmd[pid_pitch] = 0;
                cmd[pid_roll] = max;

                data.entree[pid_roll] = max;
            }
            else if (data.stabilisation_mode == 3)
            { // rampe
                data.commands[cmd_throttle] = 1550;
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

                entree[pid_roll] = cmd[pid_roll];
            }
            else if (data.stabilisation_mode == 4)
            { // sinusoidal
            commands[cmd_throttle] = 1550;
                cmd[pid_pitch] = 0;
                cmd[pid_roll] = max * cos(a * iter / 120);
                data.entree[pid_roll] = cmd[pid_roll];
            }
           
            // fix values
            float kp_rate = mapValue(commands[cmd_kp], 922, 2077, 0, pid_gains_rate_max);
            float kd_rate = mapValue(commands[cmd_ki], 921, 2077, 0, 2);

            status[status_gains_rate_kp] = kp_rate;
            status[status_gains_rate_kd] = kd_rate;

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


       // computing commands
       if(data.stabilisation_mode == 5){
           if(commands[cmd_arming] > 1400){
                data.motor_command[3] = previous_cmd;
                cmd[pid_roll] = previous_cmd;
           }
           else{
               cmd[pid_roll] = 0;
               data.motor_command[3] = 0;
               previous_cmd = data.commands[cmd_throttle];
               
               if(i%40 == 0){

               i = 0;
               std::cout << previous_cmd << "\n";
               }
               
           }
       }
        else if (isArmed && data.commands[cmd_throttle] > 1100)
        {
            if(data.stabilisation_mode != 5){
                data.motor_command[2] = data.commands[cmd_throttle] + cmd[pid_roll] - cmd[pid_pitch] - cmd[pid_yaw]; // motor 0
                data.motor_command[0] = data.commands[cmd_throttle] - cmd[pid_roll] - cmd[pid_pitch] + cmd[pid_yaw]; // motor 1
                data.motor_command[3] = data.commands[cmd_throttle] + cmd[pid_roll];                                                                      //commands[cmd_throttle] - cmd[pid_roll] * mixing + cmd[pid_pitch] * mixing - cmd[pid_yaw] * mixing; // motor 2
                data.motor_command[1] = data.commands[cmd_throttle] + cmd[pid_roll];
            }
            else {
               
            }                                                                  //commands[cmd_throttle] + cmd[pid_roll] * mixing + cmd[pid_pitch] * mixing + cmd[pid_yaw] * mixing; // motor 3
        }
        else
        {
            data.motor_command[0] = 0;
            data.motor_command[1] = 0;
            data.motor_command[2] = 0;
            data.motor_command[3] = 0;
            data.entree[0] = 0;
            data.entree[1] = 0;
            data.entree[2] = 0;
            data.entree[3] = 0;
        }
        return motor_command;
    }
};