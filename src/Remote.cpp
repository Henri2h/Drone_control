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
    static void start_remote(float *ang_in, float *time_now_in)
    {
        static float *time_pointer = time_now_in;
        static float *ang_pointer = ang_in;

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
                

                if (r.compare("#getPIDErrorPitch") == 0)
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
    void launch(float *ang, float *time_now)
    {
        first = std::thread(start_remote, ang, time_now);
    }
};