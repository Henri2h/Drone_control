#include "Filter.cpp"
#include <cmath>

class IMU_data_processing{
    private:
    const int x = 0;
    const int y = 1;
    const int z = 2;
    const float  PI = 3.1415926535; 
    
    const float kAcc = 0.02;
    const float kGyr = 0.98;

    public:
    IMU_data_processing(){}
    
    float * getAngleAccel(float * ac){
        static float ang[3]= {0, 0, 0};
        
        if(ac[z] != 0){
            ang[x] = atan(ac[x]/ac[z])*180/PI;
            ang[y] = atan(ac[y]/ac[z])*180/PI;
        }
        else { // it really mean that we cannot determine angles this way
            ang[x] = 0;
            ang[y] = 0;
        }
        
        // non trivial rules
        /*
        if(ac[z] <= 0 && ac[y] <= 0){ ang[x] -= 180; }
        else if (ac[z] <=0 && ac[y] > 0){ ang[x] += 180; }

        if(ac[z] <= 0 && ac[x] <= 0){ ang[y] -= 180; }
        else if (ac[z] <= 0 && ac[x] > 0){ ang[y] += 180; }
        */
       
        return ang;
    }

    float * getGyrationAngle(float ang[], float imuData[], float dt){
        for(size_t i = 0; i < 3; i++)
        {
            ang[i] += imuData[i + 3] * dt * 180 / PI; // to use gyration
        }
        return ang;
    }

    float * getComplementar(float * ang, float imuData[], float dt){ // ang pointer array
        float * accAng = getAngleAccel(imuData); 

         for(size_t i = 0; i < 3; i++)
        {
            ang[i] += imuData[i + 3] * dt * 180 / PI; // to use gyration
            ang[i] = ang[i] * kGyr + accAng[i] * kAcc;
        }
        return ang;

    }


};