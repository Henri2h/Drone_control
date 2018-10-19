
class Filter{
    public:
    float complementaryFilter(float a, float b, float coef_a, float coef_b){
        return a * coef_a + b * coef_b;
    }
    float low_pass_filter(float i, float e){
        return 0.0;
    }
};