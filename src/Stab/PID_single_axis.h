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

    float Kp = 0;
    float Kd = 0;
    float Ki = 0;
    float G = 5;
    float last_time = 0;

    int max_integral = 100;
    void integralWindup();
    float w0Te = 1; // produit de w0Te car w0 = 2*pi*Te
    void doFilter();

    float mapValue(float in_val, float in_min, float in_max, float out_min, float out_max);

public:
    PID_Single_Axis();
    void setPID(float in_G, float in_Kp, float in_Kd, float in_Ki);

    void setK(int kp, int kd, int ki);
    float update(float command, float feedback, float dt, float *pid_debug);
    float update(float command, float feedback, float dt);

    void displayK();
};
