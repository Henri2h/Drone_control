#include <uWS/uWS.h>
#include <iostream>
#include <cmath>

using namespace uWS;
using namespace std;

class Remote {
    void Start(){
    Hub h;
    std::string response = "Hello!";

    h.onMessage([](WebSocket<SERVER> *ws, char *message, size_t length, OpCode opCode) {
        if(opCode == OpCode::TEXT){
            cout << "selected message \n";
            for(size_t i = 0; i < length; i++)
            {
                cout << message[i];
            }

            cout << "\n";
            
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

            auto value = now_ms.time_since_epoch();
            long result = value.count();


            string str = to_string(result) ;
            char const *cstr = str.c_str();
            int size = strlen(cstr);
            cout << "date : " << cstr << " size : " << size <<"\n";
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

    if (h.listen(8766)) {
        h.run();
    }
    }
};