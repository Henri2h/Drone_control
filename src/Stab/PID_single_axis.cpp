#include "PID_single_axis.h"

void PID_Single_Axis::integralWindup()
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

void PID_Single_Axis::doFilter()
{
    float error_d_last = 0;
    error_d = (error_d_last * w0Te + error_d) / (w0Te + 1);

    // apply a windup on derivative filter to prevent excesive reaction
    if (error_d > max_integral)
    {
        error_d = max_integral;
    }
    else if (error_d < -max_integral)
    {
        error_d = -max_integral;
    }
}

PID_Single_Axis::PID_Single_Axis()
{
    cout << "[ PID Single Axis ] : Initialized\n";
}

void PID_Single_Axis::setPID(float in_G, float in_Kp, float in_Kd, float in_Ki)
{
    /// G, Kp, Kd, Ki
    G = in_G;
    Ki = in_Ki;
    Kp = in_Kp;
    Kd = in_Kd;
}

float PID_Single_Axis::update(float command, float feedback, float dt)
{
    float output = 0;

    error_p = command - feedback;
    error_d = (error_p - error_p_last) / dt;
    error_i += error_p * dt;

    //integralWindup();  // integral windup of i term
    //  doFilter(); // filter in derrivative term

    error_p_last = error_p;

    output = G * (Kp * error_p) + (Kd * error_d) + (Ki * error_i);
    return output;
}

void PID_Single_Axis::displayK()
{
    cout << "kp : " << Kp << " " << Kd << " " << Ki << "\n";
}
