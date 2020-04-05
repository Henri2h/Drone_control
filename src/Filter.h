#ifndef Filter_H
#define Filter_H

class Filter
{
    float pi = 3.14;
  public:
    float complementaryFilter(float a, float b, float coef_a, float coef_b);
    // e : entrée
    // s : sortie à t-dt
    static float low_pass_filter(float s, float e);

    static float low_pass_filter_old(float s, float o);
};

#endif
