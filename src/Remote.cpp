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
    static void start_remote(float *ang_in, float *acceleration_in, float *rates_in, int *pid_in, int *pid_debug_in, int *sensors_in, float *time_now_in)
    {
        static float *time_pointer = time_now_in;
        static float *ang_pointer = ang_in;
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
                    char const *time_str = to_string(*time_pointer).c_str();
                    char const *ang_0_str = to_string(ang_pointer[0]).c_str();
                    char const *ang_1_str = to_string(ang_pointer[1]).c_str();
                    char const *ang_2_str = to_string(ang_pointer[2]).c_str();

                    // sending data
                    ws->send(ang_0_str, strlen(ang_0_str), opCode);
                    ws->send(ang_1_str, strlen(ang_1_str), opCode);
                    ws->send(ang_2_str, strlen(ang_2_str), opCode);
                    ws->send(time_str, strlen(time_str), opCode);
                }

                else if (r.compare("#PIDErrP") == 0)
                {
                    // preparing results :
                    char const *time_str = to_string(*time_pointer).c_str();
                    char const *ang_0_str = to_string(pid_debug[0]).c_str();
                    char const *ang_1_str = to_string(pid_debug[1]).c_str();
                    char const *ang_2_str = to_string(pid_debug[2]).c_str();

                    // sending data
                    ws->send(ang_0_str, strlen(ang_0_str), opCode);
                    ws->send(ang_1_str, strlen(ang_1_str), opCode);
                    ws->send(ang_2_str, strlen(ang_2_str), opCode);
                    ws->send(time_str, strlen(time_str), opCode);
                }
                else if (r.compare("#PID") == 0)
                {
                    // preparing results :
                    char const *time_str = to_string(*time_pointer).c_str();
                    char const *ang_0_str = to_string(pid[0]).c_str();
                    char const *ang_1_str = to_string(pid[1]).c_str();
                    char const *ang_2_str = to_string(pid[2]).c_str();

                    // sending data
                    ws->send(ang_0_str, strlen(ang_0_str), opCode);
                    ws->send(ang_1_str, strlen(ang_1_str), opCode);
                    ws->send(ang_2_str, strlen(ang_2_str), opCode);
                    ws->send(time_str, strlen(time_str), opCode);
                }

                else if (r.compare("#ACC") == 0)
                {
                    // preparing results :
                    char const *time_str = to_string(*time_pointer).c_str();
                    char const *ang_0_str = to_string(acceleration[0]).c_str();
                    char const *ang_1_str = to_string(acceleration[1]).c_str();
                    char const *ang_2_str = to_string(acceleration[2]).c_str();

                    // sending data
                    ws->send(ang_0_str, strlen(ang_0_str), opCode);
                    ws->send(ang_1_str, strlen(ang_1_str), opCode);
                    ws->send(ang_2_str, strlen(ang_2_str), opCode);
                    ws->send(time_str, strlen(time_str), opCode);
                }
                else if (r.compare("#GYR") == 0)
                {
                    // preparing results :
                    char const *time_str = to_string(*time_pointer).c_str();
                    char const *ang_0_str = to_string(rates[0]).c_str();
                    char const *ang_1_str = to_string(rates[1]).c_str();
                    char const *ang_2_str = to_string(rates[2]).c_str();

                    // sending data
                    ws->send(ang_0_str, strlen(ang_0_str), opCode);
                    ws->send(ang_1_str, strlen(ang_1_str), opCode);
                    ws->send(ang_2_str, strlen(ang_2_str), opCode);
                    ws->send(time_str, strlen(time_str), opCode);
                }
                else {
                    std::cout << "[ Remote ] : command unknown : " << r << "\n";
                }
            }
        });

        h.onHttpRequest([&](HttpResponse *res, HttpRequest req, char *data, size_t length,
                            size_t remainingBytes) {
            res->end(response.data(), response.length());
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