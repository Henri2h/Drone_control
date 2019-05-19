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

    static void sendDataL(WebSocket<SERVER> *ws, double *values, float *time_pointer, int length, OpCode opCode)
    {
        // preparing results :
        char const *time_str = to_string(*time_pointer).c_str();
        for (size_t i = 0; i < length; i++)
        {
            char charray[14]; // warning : max 14 characters !!!!!
            sprintf(charray, "%f", values[i]);
            // sending data
            ws->send(charray, strlen(charray), opCode);
        }
        ws->send(time_str, strlen(time_str), opCode);
    }

    static void sendFStatus(WebSocket<SERVER> *ws, float *time_pointer, float *commands, float *gyr, float *acc, float *comp, int *pid, OpCode opCode)
    {
        /*
        0 : time
        1 : throttle
        2 : pitch
        3 : roll
        4 : yaw
        5 : gyr x
        6 : gyr y
        7 : gyr z
        8 : acc x
        9 : acc y
        10 : acc z
        11 : comp x
        12 : comp y
        13 : comp z
        14 : pid pitch
        15 : pid roll
        16 : pid yaw

        */
        // preparing results :
        char const *time_str = to_string(*time_pointer).c_str();

        // command
        char const *commands_str_0 = to_string(commands[0]).c_str();
        char const *commands_str_1 = to_string(commands[1]).c_str();
        char const *commands_str_2 = to_string(commands[2]).c_str();
        char const *commands_str_3 = to_string(commands[3]).c_str();

        // gyr
        char const *gyr_str_0 = to_string(gyr[0]).c_str();
        char const *gyr_str_1 = to_string(gyr[1]).c_str();
        char const *gyr_str_2 = to_string(gyr[2]).c_str();

        // gyr
        char const *acc_str_0 = to_string(acc[0]).c_str();
        char const *acc_str_1 = to_string(acc[1]).c_str();
        char const *acc_str_2 = to_string(acc[2]).c_str();

        // gyr
        char const *comp_str_0 = to_string(comp[0]).c_str();
        char const *comp_str_1 = to_string(comp[1]).c_str();
        char const *comp_str_2 = to_string(comp[2]).c_str();

        // pid
        char const *pid_str_0 = to_string(pid[0]).c_str();
        char const *pid_str_1 = to_string(pid[1]).c_str();
        char const *pid_str_2 = to_string(pid[2]).c_str();

        // sending data
        ws->send(time_str, strlen(time_str), opCode); // time

        // commands
        ws->send(commands_str_0, strlen(commands_str_0), opCode);
        ws->send(commands_str_1, strlen(commands_str_1), opCode);
        ws->send(commands_str_2, strlen(commands_str_2), opCode);
        ws->send(commands_str_3, strlen(commands_str_3), opCode);

        // gyr
        ws->send(gyr_str_0, strlen(gyr_str_0), opCode);
        ws->send(gyr_str_1, strlen(gyr_str_1), opCode);
        ws->send(gyr_str_2, strlen(gyr_str_2), opCode);

        // acc
        ws->send(acc_str_0, strlen(acc_str_0), opCode);
        ws->send(acc_str_1, strlen(acc_str_1), opCode);
        ws->send(acc_str_2, strlen(acc_str_2), opCode);

        // comp
        ws->send(comp_str_0, strlen(comp_str_0), opCode);
        ws->send(comp_str_1, strlen(comp_str_1), opCode);
        ws->send(comp_str_2, strlen(comp_str_2), opCode);

        // comp
        ws->send(pid_str_0, strlen(pid_str_0), opCode);
        ws->send(pid_str_1, strlen(pid_str_1), opCode);
        ws->send(pid_str_2, strlen(pid_str_2), opCode);
    }

    static void start_remote(float *commands_in, float *ang_in, float *acceleration_in, float *rates_in, int *pid_in, float *pid_debug_in, int *sensors_in, double *status_in, int *orders_in, float *time_now_in)
    {
        static float *commands = commands_in;
        static float *time_pointer = time_now_in;
        static float *ang = ang_in;
        static int *pid = pid_in;
        static float *pid_debug = pid_debug_in; // not used
        static int *sensors = sensors_in;

        static float *acceleration = acceleration_in;
        static float *rates = rates_in;
        static int *orders = orders_in;
        static double *status = status_in;

        FileManagement::Log("REMOTE", "Started");
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
                else if (r.compare("#FStatus") == 0)
                {
                    sendFStatus(ws, time_pointer, commands, rates, acceleration, ang, pid, opCode);
                }
                else if (r.compare("#Status") == 0)
                {
                    sendDataL(ws, status, time_pointer, status_length, opCode);
                }
                else if (r.compare("#ListFiles") == 0)
                {
                    // nothing
                    std::cout << "#ListFiles\n";
                }
                else if (r.compare("#OrdRecordOn") == 0)
                {
                    // nothing
                    std::cout << "OrdRecordOn\n";
                    orders[0] = 1;
                    ws->send("OK", 2, opCode);
                }
                else if (r.compare("#OrdRecordOff") == 0)
                {
                    // nothing
                    std::cout << "OrdRecordOff\n";
                    orders[0] = 0;
                    ws->send("OK", 2, opCode);
                }

                // DK : display gains
                else if (r.compare("#OrdDKOn") == 0)
                {
                    // nothing
                    orders[1] = 1;
                    ws->send("OK", 2, opCode);
                }
                else if (r.compare("#OrdDKOff") == 0)
                {
                    // nothing
                    orders[orders_DisplayGains] = 0;
                    ws->send("OK", 2, opCode);
                }

                // File Management :
                else if (r.compare("#FL"))
                { // FL : FileList
                    std::string strlist = FileManagement::listDir("log");
                    char const *data = strlist.c_str();
                    ws->send(data, strlen(data), opCode);
                }

                else if (r.compare("#FS"))
                { // FL : FileSend
                }

                else
                {
                   FileManagement::Log("Remote", "command unknown");
                    ws->send("KO", 2, opCode);
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
    void launch(float *commands_in, float *ang, float *acceleration, float *rates, int *pid, float *pid_debug, int *sensors, double *status, int *orders, float *time_now)
    {
        first = std::thread(start_remote, commands_in, ang, acceleration, rates, pid, pid_debug, sensors, status, orders, time_now);
    }
};