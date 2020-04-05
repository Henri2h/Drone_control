#ifndef SensorManager_H
#define SensorManager_H

#include "GPS.h"
#include "Barometer.h"
#include <thread>

class SensorManager
{
private:
  static bool isLooping;
  thread sensor_thread;

  static void thread_update_loop(double *status);

public:
  SensorManager();
  void startThread(double *status);
  void stropThread();
};

#endif