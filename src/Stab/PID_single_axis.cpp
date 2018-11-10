#include <unistd.h>
#include <memory>
#include <iostream>
using namespace std;

class PID_Single_Axis
{
  private:
    float error_p = 0;
    float error_d = 0;
    float error_i = 0;

    float error_p_last = 0;
    float error_d_last = 0;
    float error_i_last = 0;

    float Kp = 0;
    float Kd = 0;
    float Ki = 0;
    float G = 1;
    float last_time = 0;

    int max_integral = 100;
    void integralWindup()
    {
        for (int i = 0; i < 3; i++)
        {
            if (error_i > max_integral)
            {
                error_i = max_integral;
            }
            else if (error_i < -max_integral)
            {
                error_i = -max_integral;
            }
        }
    }

    float w0Te = 20; // produit de w0Te car w0 = 2*pi*Te
    void doFilter()
    {
        error_d = (error_d_last * w0Te + error_d) / (w0Te + 1);

        if (error_d > max_integral)
        {
            error_d = max_integral;
        }
        else if (error_d < -max_integral)
        {
            error_d = -max_integral;
        }
    }

    float mapValue(float in_val, float in_min, float in_max, float out_min, float out_max)
    {
        float out_val = (out_max - out_min) / (in_max - in_min) * (in_val - in_min) + out_min;
        return out_val;
    }

  public:
    PID_Single_Axis()
    {
        cout << "[ PID Single Axis ] : Initialized\n";
    }
    void setPID(float in_G, float in_Kp, float in_Kd, float in_Ki)
    {
        G = in_G;
        Ki = in_Ki;
        Kp = in_Kp;
        Kd = in_Kd;
    }

    void setK(int kp, int kd, int ki)
    {
        bool enabled = false;
        if (enabled)
        {
            Kp = mapValue(kp, 922, 2077, 0, 4);
            Kd = mapValue(kd, 922, 2077, 0, 1);
            Ki = mapValue(ki, 921, 2077, 0, 1);
        }
        //  Kp[1] = mapValue(kp, 1000, 2000, 0, 100);
        //  Kd[1] = mapValue(kd, 1000, 2000, 0, 100);
        //  Ki[1] = mapValue(ki, 1000, 2000, 0, 100);
    }

    float update(float command, float feedback, float dt)
    {
        float output = 0;

        error_p = command - feedback;
        // error_d[i] = (error_p[i] - error_p_last[i]) / dt;
        error_d = (-error_p_last) / dt;
        error_i += error_p * dt;
        // cout << "i : " << i << " : " << mapValue(commands[i], 1000, 2000, -30, 30) << " error : " << error_p[i] << " ";

        //        cout << "\n";

        integralWindup();
        doFilter();

        error_p_last = error_p;
        error_d_last = error_d;
        error_i_last = error_i;

        output = G * (Kp * error_p) + (Kd * error_d) + (Ki * error_i);
        return output;
    }
    void displayK()
    {
        cout << "kp : " << Kp << " " << Kd << " " << Ki << "\n";
    }
};