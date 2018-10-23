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

    int max_integral = 500;
    void integralWindup()
    {
        for (int i = 0; i < 3; i++)
        {
            if (error_i[i] > max_integral)
            {
                error_i[i] = max_integral;
            }
            else if (error_i[i] < -max_integral)
            {
                error_i[i] = -max_integral;
            }
        }
    }

    float w0Te = 3.14 * 2; // produit de w0Te car w0 = 2*pi*Te
    void doFilter()
    {
        for (size_t i = 0; i < 3; i++)
        {
            error_d[i] = (error_d_last[i] * w0Te + error_d[i]) / (w0Te + 1);
        }
    }

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

    void setK(int kp, int kd, int ki)
    {
        
        Kp[0] = mapValue(kp, 922, 2077, 0, 4);
        Kd[0] = mapValue(kd, 922, 2077, 0, 1);
        Ki[0] = mapValue(ki, 921, 2077, 0, 1);
       
        //  Kp[1] = mapValue(kp, 1000, 2000, 0, 100);
        //  Kd[1] = mapValue(kd, 1000, 2000, 0, 100);
        //  Ki[1] = mapValue(ki, 1000, 2000, 0, 100);
    }

    int *getPID(int output[], int debug[], int commands[], float feedbacks[], float dt)
    {
        for (int i = 0; i < 3; i++)
        {
            error_p[i] = mapValue(commands[i], 1075, 1920, -30, 30) - feedbacks[i];
            error_d[i] = (error_p[i] - error_p_last[i]) / dt;
            error_i[i] += error_p[i] * dt;
            // cout << "i : " << i << " : " << mapValue(commands[i], 1000, 2000, -30, 30) << " error : " << error_p[i] << " ";
        }

        //        cout << "\n";

        integralWindup();
        doFilter();

        // save
        for (int i = 0; i < 3; i++)
        {
            error_p_last[i] = error_p[i];
            error_d_last[i] = error_d[i];
            error_i_last[i] = error_i[i];
        }

        for (int i = 0; i < 3; i++)
        {
            output[i] = (Kp[i] * error_p[i]) + (Kd[i] * error_d[i]) + (Ki[i] * error_i[i]);
        }

        debug[0] = Kp[0] * error_p[0];
        debug[1] = Kd[0] * error_d[0];
        debug[2] = Ki[0] * error_i[0];

        return output;
    }
    void displayK(){
         cout << "kp : " << Kp[0] << " " << Kd[0] << " " << Ki[0] << "\n";
    }
};