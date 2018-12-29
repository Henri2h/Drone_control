class Attitude_Controller
{
  private:
    // error
    float error[3] = {0, 0, 0};
    float Kp[3] = {0, 0, 0};
    float error_last[3] = {0, 0, 0};

    PID_Single_Axis loop_atti_pitch = PID_Single_Axis();
    PID_Single_Axis loop_rate_pitch = PID_Single_Axis();

    PID_Single_Axis loop_atti_roll = PID_Single_Axis();
    PID_Single_Axis loop_rate_roll = PID_Single_Axis();

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
        loop_atti_pitch.setPID(0.01, 250, 0, 0);
        loop_atti_roll.setPID(0.01, 250, 0, 0); // according to librepilot

        loop_rate_pitch.setPID(1, 0.006, 0.000081, 0.018);
        loop_rate_roll.setPID(1, 0.0054, 0.0081, 0.016);

        loop_rate_yaw.setPID(1, 0.00897, 0.000076, 0.01705);
        // setup gains for the controllers (Kp)
    }

    void update(float *cmd, float *ang, float *rates, float dt, float *pid_debug)
    {        
        cmd[pid_pitch] = loop_atti_pitch.update(cmd[pid_pitch], ang[pid_pitch], dt); // outer loop (attitude)
        cmd[pid_pitch] = loop_rate_pitch.update(cmd[pid_pitch], rates[pid_pitch], dt);     //inner loop (rate)

        cmd[pid_roll] = loop_atti_roll.update(cmd[pid_roll], ang[pid_roll], dt); // outer loop (attitude)
        cmd[pid_roll] = loop_rate_roll.update(cmd[pid_roll], rates[pid_roll], dt, pid_debug);     //inner loop (rate)

        cmd[pid_yaw] = loop_rate_yaw.update(cmd[pid_yaw], rates[pid_yaw], dt);
    }

    void update_pid(float kp_atti, float kp_rate, float kd_rate){
        
        loop_atti_pitch.setPID(0.01, kp_atti, 0, 0);
        loop_atti_roll.setPID(0.01, kp_atti, 0, 0); // according to librepilot

        loop_rate_pitch.setPID(0.01, kp_rate, kd_rate, 0);
        loop_rate_roll.setPID(0.01, kp_rate, kd_rate, 0);        
    }
};