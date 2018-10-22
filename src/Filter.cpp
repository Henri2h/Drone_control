
class Filter
{
  public:
    float complementaryFilter(float a, float b, float coef_a, float coef_b)
    {
        return a * coef_a + b * coef_b;
    }
    float low_pass_filter(float Vsn, float Ven)
    {
        // according to shannon criteria,
        float w0Te = 3.14 / 10; // produit de w0Te car w0 = 2*pi*Te
        float vsn = (Ven * w0Te + Vsn) / (w0Te + 1);
        return vsn;
    }
};