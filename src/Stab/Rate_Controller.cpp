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
    Rate_Controller()
    {
        // setup gains for the controllers
        loop_rate_pitch.setPID(0.01, 192, 0, 0);
        loop_rate_roll.setPID(0.01, 192, 0, 0);
        loop_rate_yaw.setPID(0.01, 150, 0, 0);
    }

    void update(float *cmd, float *rates, float dt, float *pid_debug)
    {
        // inner loop
        cmd[pid_pitch] = loop_rate_pitch.update(cmd[pid_pitch], rates[pid_pitch], dt);
        cmd[pid_roll] = loop_rate_roll.update(cmd[pid_roll], rates[pid_roll], dt, pid_debug);
        cmd[pid_yaw] = loop_rate_yaw.update(cmd[pid_yaw], rates[pid_yaw], dt);
    }

     void update_pid(float kp_rate, float kd_rate){
        loop_rate_pitch.setPID(0.01, kp_rate, kd_rate, 0);
        loop_rate_roll.setPID(0.01, kp_rate, kd_rate, 0);        
    }
};