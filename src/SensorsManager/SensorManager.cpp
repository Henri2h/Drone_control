#include "GPS.cpp"
#include "Barometer.cpp"

class SensorManager
{
  private:
    
    static bool  isLooping;
    std::thread sensor_thread;



    static void thread_update_loop(float *status)
    {
        GPS gps = GPS();
     Barometer baro = Barometer();

        gps.initialize(status);
        baro.initialize(status);

        while (SensorManager::isLooping)
        {
            gps.update();
            baro.update();
            sleep(0.5);
        }
    }

  public:
    void startThread(float *status)
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