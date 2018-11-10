#include "math.h"
float mapValue(float in_val, float in_min, float in_max, float out_min, float out_max)
{
    float out_val = (out_max - out_min) / (in_max - in_min) * (in_val - in_min) + out_min;
    return out_val;
}
