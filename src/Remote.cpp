#include <uWS/uWS.h>
#include <iostream>
#include <cmath>
#include <thread>

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
                char mess[length];
                for (size_t i = 0; i < length; i++)
                {
                    mess[i] = message[i];
                }
                if (strcmp(mess, "#getErrorPitch") == 0)
                {
                    cout << "same\n";
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

                auto now = std::chrono::system_clock::now();
                auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

                auto value = now_ms.time_since_epoch();
                long result = value.count();

                string str = to_string(result);
                char const *cstr = str.c_str();
                int size = strlen(cstr);
                cout << "date : " << cstr << " size : " << size << "\n";
                int random_variable = std::rand();

                // over complicated code
                std::string s = std::to_string(random_variable);
                char const *pchar = s.c_str();

                ws->send(pchar, strlen(pchar), opCode);
                ws->send("8", 1, opCode);
                ws->send("3", 1, opCode);
                ws->send(cstr, size, opCode);
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