class Attitude_Controller
{
  private:
    // error
    float error[3] = {0, 0, 0};
    float Kp[3] = {0, 0, 0};
    float error_last[3] = {0, 0, 0};

  public:
    Attitude_Controller()
    {
        // setup gains for the controllers (Kp)
    }

    void update(float *cmd, float *ang, float dt)
    {

        for (size_t i = 0; i < 3; i++)
        {
            error[i] = (cmd[i] - ang[i]) * Kp[i];
            // now we need to derivate

            cmd[i] = (error[i] - error_last[i]) / dt;
            error_last[i] = error[i];
        }
    }
};