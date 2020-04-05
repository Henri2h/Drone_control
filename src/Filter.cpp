#include "Filter.h"

float Filter::complementaryFilter(float a, float b, float coef_a, float coef_b)
{
    return a * coef_a + b * coef_b;
}

// e : entrée
// s : sortie à t-dt
float Filter::low_pass_filter(float s, float e)
{
    // according to shannon criteria,
    //float rapport = pi / 10; // produit de w0Te car w0 = 2*pi*Te car10T = dt*
    float rapport = 20; // w0Te = 10

    float vsn = (e * rapport + s) / (rapport + 1);
    return vsn;
}

float Filter::low_pass_filter_old(float s, float o)
{
    // according to shannon criteria,
    float alpha = 0.01; // produit de w0Te car w0 = 2*pi*Te car10T = dt*
    // w0Te = 10

    return o + alpha * (s - o); //  y[i] := y[i-1] + α * (x[i] - y[i-1])
}
