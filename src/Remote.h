#ifndef Remote_H
#define Remote_H

#include <uWS/uWS.h>
#include <iostream>
#include <cmath>
#include <thread>
#include <sstream>

#include "Data.h"
#include "FileManagement.h"

using namespace uWS;
using namespace std;

class Remote
{
private:
	static Data data;

	static void sendData(WebSocket<SERVER> *ws, float *values, float *time_pointer, OpCode opCode);
	static void sendData(WebSocket<SERVER> *ws, int *values, float *time_pointer, OpCode opCode);
	static void sendDataL(WebSocket<SERVER> *ws, Data *dt, float *time_pointer, int length, OpCode opCode);
	static void sendFStatus(WebSocket<SERVER> *ws, Data *dt, float *time_pointer, OpCode opCode);

	void sendGains(WebSocket<SERVER> *ws, Data *values, float *time_pointer, int length, OpCode opCode);

	static std::vector<std::string> split(const std::string &s, char delimiter);

	static void start_remote(Data *data_i, float *time_now_in);
	pthread_t remote_thread;

public:
	std::thread first;
	void launch(Data *data_in, float *time_now);
};

#endif
