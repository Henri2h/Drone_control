
class Filter
{
    float pi = 3.14;
  public:
    float complementaryFilter(float a, float b, float coef_a, float coef_b)
    {
        return a * coef_a + b * coef_b;
    }

    // e : entrée
    // s : sortie à t-dt
    float low_pass_filter(float s, float e)
    {
        // according to shannon criteria,
        float rapport = pi / 10; // produit de w0Te car w0 = 2*pi*Te car10T = dt*
        rapport = 20; // w0Te = 10

        float vsn = (e * rapport + s) / (rapport + 1);
        return vsn;
    }
};