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
    Attitude_Controller()
    {
        loop_pos_pitch.setPID(0.01, 250, 0, 0);
        loop_pos_roll.setPID(0.01, 250, 0, 0); // according to librepilot

        loop_speed_pitch.setPID(1, 0.006, 0.000081, 0.018);
        loop_speed_roll.setPID(1, 0.0054, 0.0081, 0.016);

        loop_rate_yaw.setPID(1, 0.00897, 0.000076, 0.01705);
        // setup gains for the controllers (Kp)
    }

    float cmd_last_pitch = 0;
    float cmd_last_roll = 0.0;
    float e_s = 0;

    // attention, pour permettre que les deux boucles soient indépendantes, il ne faut pas quelle fonctionnent à la même fréquence !! 
    void update(Data& data, float *cmd, float dt)
    {   
        // pitch
        cmd[pid_pitch] = loop_pos_pitch.update(cmd[pid_pitch], data.ang[pid_pitch], dt); // outer loop (attitude)
        cmd[pid_pitch] = loop_speed_pitch.update(cmd[pid_pitch], data.rates[pid_pitch], dt);     //inner loop (rate)




        // roll
        float pid_cmd_roll = cmd[pid_roll];
            // angle
        pid_cmd_roll = loop_pos_roll.update(pid_cmd_roll, data.ang[pid_roll], dt); // outer loop pos
        e_s = e_s + pid_cmd_roll*dt;
        // feed it to the inner loop
        
        cmd[pid_roll] = loop_speed_roll.update(e_s, data.rates[pid_roll], dt, data.pid_debug);     //inner loop speed
        // std::cout << "pid cmd  rool outer loop : " << pid_cmd_roll << " out cmd[] : " << cmd[pid_roll] << " " << "\n";
        
        // last
        cmd_last_roll = pid_cmd_roll;

        if(isnan(cmd[pid_roll])){
            cmd[pid_roll] = 0;
            std::cout << "isnan\n";
        }

        cmd[pid_yaw] = loop_rate_yaw.update(cmd[pid_yaw], data.rates[pid_yaw], dt);
    }

    void update_pid(float kp_atti, float kp_rate, float kd_rate){
        
        loop_pos_pitch.setPID(0.01, kp_rate, kd_rate, 0);
        loop_speed_pitch.setPID(0.01, kp_atti, 0, 0);       

        loop_pos_roll.setPID(0.01,  kp_rate, kd_rate, 0); // according to librepilot
        loop_speed_roll.setPID(0.01, kp_atti, 0, 0); 

    }
};