#include <uWS/uWS.h>
#include <iostream>
#include <cmath>
#include <thread>
#include <sstream>

using namespace uWS;
using namespace std;

class Remote
{
  private:
    static void sendData(WebSocket<SERVER> *ws, float *values, float *time_pointer, OpCode opCode)
    {
        // preparing results :
        char const *time_str = to_string(*time_pointer).c_str();
        char const *ang_0_str = to_string(values[0]).c_str();
        char const *ang_1_str = to_string(values[1]).c_str();
        char const *ang_2_str = to_string(values[2]).c_str();

        // sending data
        ws->send(ang_0_str, strlen(ang_0_str), opCode);
        ws->send(ang_1_str, strlen(ang_1_str), opCode);
        ws->send(ang_2_str, strlen(ang_2_str), opCode);
        ws->send(time_str, strlen(time_str), opCode);
    }
    static void sendData(WebSocket<SERVER> *ws, int *values, float *time_pointer, OpCode opCode)
    {
        // preparing results :
        char const *time_str = to_string(*time_pointer).c_str();
        char const *ang_0_str = to_string(values[0]).c_str();
        char const *ang_1_str = to_string(values[1]).c_str();
        char const *ang_2_str = to_string(values[2]).c_str();

        // sending data
        ws->send(ang_0_str, strlen(ang_0_str), opCode);
        ws->send(ang_1_str, strlen(ang_1_str), opCode);
        ws->send(ang_2_str, strlen(ang_2_str), opCode);
        ws->send(time_str, strlen(time_str), opCode);
    }

    static void start_remote(float *ang_in, float *acceleration_in, float *rates_in, int *pid_in, int *pid_debug_in, int *sensors_in, float *time_now_in)
    {
        static float *time_pointer = time_now_in;
        static float *ang = ang_in;
        static int *pid = pid_in;
        static int *pid_debug = pid_debug_in;
        static int *sensors = sensors_in;

        static float *acceleration = acceleration_in;
        static float *rates = rates_in;

        printf("[ REMOTE ] : Started\n");
        Hub h;
        std::string response = "Hello!";

        h.onMessage([](WebSocket<SERVER> *ws, char *message, size_t length, OpCode opCode) {
            if (opCode == OpCode::TEXT)
            {
                std::ostringstream os;
                for (size_t i = 0; i < length; i++)
                {
                    os << message[i];
                }
                string r = string(os.str());

                if (r.compare("#AngComp") == 0)
                {
                    // preparing results :
                    sendData(ws, ang, time_pointer, opCode);
                }

                else if (r.compare("#PIDErrP") == 0)
                {
                    sendData(ws, pid_debug, time_pointer, opCode);
                }
                else if (r.compare("#PID") == 0)
                {
                    sendData(ws, pid, time_pointer, opCode);
                }

                else if (r.compare("#Acc") == 0)
                {
                    sendData(ws, acceleration, time_pointer, opCode);
                }
                else if (r.compare("#Gyr") == 0)
                {
                    sendData(ws, rates, time_pointer, opCode);
                }
                else
                {
                    std::cout << "[ Remote ] : command unknown : " << r << "\n";
                }
            }
        });

        if (h.listen(8766))
        {
            h.run();
        }
        pthread_exit(NULL);
    }
    pthread_t remote_thread;

  public:
    std::thread first;
    void launch(float *ang, float *acceleration, float *rates, int *pid, int *pid_debug, int *sensors, float *time_now)
    {
        first = std::thread(start_remote, ang, acceleration, rates, pid, pid_debug, sensors, time_now);
    }
};