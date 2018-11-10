#include "PID_single_axis.cpp"

class Rate_Controller
{
    PID_Single_Axis loop_pitch = PID_Single_Axis();
    PID_Single_Axis loop_roll = PID_Single_Axis();
    // error
    float error[3] = {0, 0, 0};

  public:
    Rate_Controller()
    {
        // setup gains for the controllers
    }

    void update(float *cmd, float *rates, float dt)
    {
        for (size_t i = 0; i < 3; i++)
        {
            error[i] = (cmd[i] - rates[i]);
        }
        float out_pitch = loop_pitch.update(error[pid_pitch], dt);
        float out_roll = loop_roll.update(error[pid_roll], dt);
    }
};