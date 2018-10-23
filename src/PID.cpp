#include <unistd.h>
#include <memory>
#include <iostream>
using namespace std;

class PID
{
  private:
    float error_p[3] = {0, 0, 0};
    float error_d[3] = {0, 0, 0};
    float error_i[3] = {0, 0, 0};

    float error_p_last[3] = {0, 0, 0};
    float error_d_last[3] = {0, 0, 0};
    float error_i_last[3] = {0, 0, 0};

    float Kp[3] = {0, 0, 0};
    float Kd[3] = {0, 0, 0};
    float Ki[3] = {0, 0, 0};
    float last_time = 0;

    void integralWindup() {}
    void filter() {}

    float mapValue(float in_val, float in_min, float in_max, float out_min, float out_max)
    {
        float out_val = (out_max - out_min) / (in_max - in_min) * (in_val - in_min) + out_min;
        return out_val;
    }

  public:
    PID()
    {
        cout << "[ PID ] : initialized\n";
    }

    int *getPID(int output[], int commands[], float feedbacks[], float time_in)
    {
        for (int i = 0; i < 3; i++)
        {
            error_p[i] = commands[i] - feedbacks[i];
        }

        integralWindup();
        filter();

        // save
        for (int i = 0; i < 3; i++)
        {
            error_p_last[i] = error_p[i];
            error_d_last[i] = error_d[i];
            error_i_last[i] = error_i[i];
        }
        last_time = time_in;

        for (int i = 0; i < 3; i++)
        {
            output[i] = (Kp[i] * error_p[i]) + (Kd[i] * error_d[i]) + (Ki[i] * error_i[i]);
        }

        return output;
    }
};