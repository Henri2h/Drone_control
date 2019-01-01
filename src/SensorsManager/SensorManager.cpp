#include "GPS.cpp"
#include "Barometer.cpp"

class SensorManager
{
  private:
    
    static bool  isLooping;
    std::thread sensor_thread;



    static void thread_update_loop(double *status)
    {
        GPS gps = GPS();
     Barometer baro = Barometer();

        gps.initialize(status);
        baro.initialize(status);

        while (SensorManager::isLooping)
        {
            gps.update();
            baro.update();
           // std::cout << "baro pressure : " << status[status_baro_pressure] << " temperature : " << status[status_baro_temperature] << "\n";
            sleep(0.5);
          
         /*   for(size_t i = 0; i < status_length; i++)
            {
                std::cout << "status : " << i << " : " << status[i] << "\n";
            }
           */ 
        }
    }

  public:
    void startThread(double *status)
    {
         SensorManager::isLooping = true;
        sensor_thread = std::thread(thread_update_loop, status);
    }
    void stropThread()
    {
        SensorManager::isLooping = false;
    }
    
};
bool SensorManager::isLooping = false;