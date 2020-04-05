#ifndef PID_H
#define PID_H

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
    int max_integral = 100;
    float w0Te = 20; // produit de w0Te car w0 = 2*pi*Te

    void integralWindup();
    void doFilter();
    float mapValue(float in_val, float in_min, float in_max, float out_min, float out_max);

  public:
    PID();
    void setK(int kp, int kd, int ki);
    int *getPID(int output[], int debug[], int commands[], float feedbacks[],  float dt);
    void displayK();
};

#endif
