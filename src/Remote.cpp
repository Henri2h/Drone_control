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
	static Data data;

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

	static void sendDataL(WebSocket<SERVER> *ws, Data *dt, float *time_pointer, int length, OpCode opCode)
	{
		// preparing results :
		char const *time_str = to_string(*time_pointer).c_str();
		for (size_t i = 0; i < length; i++)
		{
			char charray[14]; // warning : max 14 characters !!!!!
			//cout << "i : " << i << " : " << dt->status[i] << " ";
			char const *val = to_string(dt->status[i]).c_str();
			// sending data
			ws->send(val, strlen(val), opCode);
		}
		ws->send(time_str, strlen(time_str), opCode);
	}

	static void sendFStatus(WebSocket<SERVER> *ws, Data *dt, float *time_pointer, OpCode opCode)
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
		char const *commands_str_0 = to_string(dt->commands[0]).c_str();
		char const *commands_str_1 = to_string(dt->commands[1]).c_str();
		char const *commands_str_2 = to_string(dt->commands[2]).c_str();
		char const *commands_str_3 = to_string(dt->commands[3]).c_str();

		// gyr
		char const *gyr_str_0 = to_string(dt->rates[0]).c_str();
		char const *gyr_str_1 = to_string(dt->rates[1]).c_str();
		char const *gyr_str_2 = to_string(dt->rates[2]).c_str();

		// gyr
		char const *acc_str_0 = to_string(dt->acceleration[0]).c_str();
		char const *acc_str_1 = to_string(dt->acceleration[1]).c_str();
		char const *acc_str_2 = to_string(dt->acceleration[2]).c_str();

		// gyr
		char const *comp_str_0 = to_string(dt->ang[0]).c_str();
		char const *comp_str_1 = to_string(dt->ang[1]).c_str();
		char const *comp_str_2 = to_string(dt->ang[2]).c_str(); // comp ang

		// pid
		char const *pid_str_0 = to_string(dt->controller_gains[0]).c_str();
		char const *pid_str_1 = to_string(dt->controller_gains[1]).c_str();
		char const *pid_str_2 = to_string(dt->controller_gains[2]).c_str();

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

	static std::vector<std::string> split(const std::string &s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}

	static void start_remote(Data *data_i, float *time_now_in)
	{
		static Data *dt = data_i;
		static float *time_pointer = time_now_in;

		FileManagement::Log("REMOTE", "Started");
		Hub h;

		h.onMessage([](WebSocket<SERVER> *ws, char *message, size_t length, OpCode opCode) {
			if (opCode == OpCode::TEXT)
			{
				std::ostringstream os;
				for (size_t i = 0; i < length; i++)
				{
					os << message[i];
				}
				string r = string(os.str());

				std::vector<std::string> L_r = split(r, ' ');

				if (L_r.size() == 1)
				{
					if (L_r[0].compare("#ExpAbort") == 0)
					{
						dt->status[status_experience_mode] = 0;
						FileManagement::Log("Remote", "Experience aborted");
						dt->orders[0] = 0; // stop saving
						ws->send("OK", 2, opCode);
					}
					else if (L_r[0].compare("#ExpStart") == 0)
					{
						FileManagement::Log("Remote", "Experience started");
						dt->status[status_experience_mode] = 1;
						ws->send("OK", 2, opCode);
					}

					else if (L_r[0].compare("#FStatus") == 0)
					{
						sendFStatus(ws, dt, time_pointer, opCode);
					}
					else if (L_r[0].compare("#Status") == 0)
					{
						sendDataL(ws, dt, time_pointer, status_length, opCode);
					}

					else if (r.compare("#OrdRecordOn") == 0)
					{
						// nothing
						std::cout << "OrdRecordOn\n";
						dt->orders[0] = 1;
						ws->send("OK", 2, opCode);
					}
					else if (r.compare("#OrdRecordOff") == 0)
					{
						// nothing
						std::cout << "OrdRecordOff\n";
						dt->orders[0] = 0;
						ws->send("OK", 2, opCode);
					}

					// DK : display gains
					else if (r.compare("#OrdDKOn") == 0)
					{
						// nothing
						dt->orders[1] = 1;
						ws->send("OK", 2, opCode);
					}
					else if (r.compare("#OrdDKOff") == 0)
					{
						// nothing
						dt->orders[orders_DisplayGains] = 0;
						ws->send("OK", 2, opCode);
					}

					// File Management :
					else if (r.compare("#FL"))
					{ // FL : FileList
						std::string strlist = FileManagement::listDir("/home/pi/log");
						char const *data = strlist.c_str();
						ws->send(data, strlen(data), opCode);
					}

					else if (r.compare("#FS"))
					{ // FL : FileSend
					}

					else if (r.compare("#AngComp") == 0)
					{
						sendData(ws, dt->ang, time_pointer, opCode);
					}

					else if (r.compare("#PIDErrP") == 0)
					{
						sendData(ws, dt->pid_debug, time_pointer, opCode);
					}
					else if (r.compare("#PID") == 0)
					{
						sendData(ws, dt->pid_out, time_pointer, opCode);
					}

					else if (r.compare("#Acc") == 0)
					{
						sendData(ws, dt->acceleration, time_pointer, opCode);
					}
					else if (r.compare("#Gyr") == 0)
					{
						sendData(ws, dt->rates, time_pointer, opCode);
					}

					else if (r.compare("#ListFiles") == 0)
					{
						// nothing
						std::cout << "#ListFiles\n";
					}

					else
					{
						FileManagement::Log("Remote", "command unknown : |" + r + "|");
						ws->send("KO", 2, opCode);
					}
				}
				else if (L_r.size() > 1)
				{

					if (L_r[0].compare("#setFilter") == 0)
					{ // set filter
						if (L_r[1].compare("NONE") == 0)
						{
							dt->stabilisation_mode = IMU_Filter_usage_none;
							dt->status[status_filter_mode] = IMU_Filter_usage_none;
							FileManagement::Log("Remote", "Filter mode none");
							ws->send("OK", 2, opCode);
						}
						else if (L_r[1].compare("ACC") == 0)
						{
							dt->stabilisation_mode = IMU_Filter_usage_acc;
							dt->status[status_filter_mode] = IMU_Filter_usage_acc;
							FileManagement::Log("Remote", "Filter mode acc");
							ws->send("OK", 2, opCode);
						}
						else if (L_r[1].compare("GYR") == 0)
						{
							dt->stabilisation_mode = IMU_Filter_usage_gyr;
							dt->status[status_filter_mode] = IMU_Filter_usage_gyr;
							FileManagement::Log("Remote", "Filter mode gyr");
							ws->send("OK", 2, opCode);
						}
						else if (L_r[1].compare("BOTH") == 0)
						{
							dt->stabilisation_mode = IMU_Filter_usage_both;
							dt->status[status_filter_mode] = IMU_Filter_usage_both;
							FileManagement::Log("Remote", "Filter both");
							ws->send("OK", 2, opCode);
						}
						else
						{
							FileManagement::Log("Remote", "setFilter : parameter unknown : |" + L_r[0] + "|" + L_r[1] + "|");
							ws->send("KO", 2, opCode);
						}
					}
					else if (L_r[0].compare("#OrdRecord") == 0)
					{
						if (L_r[1].compare("OFF") == 0)
						{
							// nothing
							std::cout << "OrdRecordOff\n";
							dt->orders[0] = 0;
							ws->send("OK", 2, opCode);
						}
						else if (L_r[1].compare("ON") == 0)
						{
							// nothing
							std::cout << "OrdRecordOn\n";
							dt->orders[0] = 1;
							ws->send("OK", 2, opCode);
						}
					}
					else if (L_r[0].compare("#SetGainsRate") == 0)
					{
						
						cout << L_r.size << "arguments : 1: " << L_r[1] << " 2: " << L_r[2] << "\n";
						int index = std::stoi(L_r[1]);
						// set values
						dt->controller_gains[index] = std::stof(L_r[1]);
						cout << "Set gains : " << dt->controller_gains[index] << "\n";
						ws->send("OK", 2, opCode);
					}
					else
					{
						FileManagement::Log("Remote", "command unknown : |" + L_r[0] + "|" + L_r[1] + "|");
						ws->send("KO", 2, opCode);
					}
				}
				else
				{
					FileManagement::Log("Remote", "command unknown : |" + r + "|");
					cout << "L_r size : " << L_r.size() << "\n";
					ws->send("KO", 2, opCode);
				}
			}
		});

		if (h.listen(8766))
		{
			h.run();
			FileManagement::Log("Remote", "Started");
		}
		pthread_exit(NULL);
	}
	pthread_t remote_thread;

public:
	std::thread first;
	void launch(Data *data_in, float *time_now)
	{
		first = std::thread(start_remote, data_in, time_now);
	}
};