/*
Title : Drone control
Description : School project
Author : made by Henri2h
Year : 2018 - 2020
*/
#include "Drone.h"

void updateOrders()
{
	if (data.orders[status_Saving] == 1)
	{
		//std::cout << "[ Saving ] :  Send saving order ...\n";
		data.status[status_Saving] = 1;
	}
	else
	{
		//std::cout << "[ Saving ] :  Send stop saving order ...\n";
		data.status[status_Saving] = 0;
	}

	if (data.orders[status_DisplayGains] == 1)
	{
		stab->showGains = true;
	}
	else
	{
		stab->showGains = false;
	}
}

void safety()
{
	//cout << "throttle : " << commands[cmd_throttle] << " arming : " << commands[cmd_arming] <<  "\n";
	if (data.commands[cmd_throttle] < 1100) // evaluation must be done at 0 throttle !!!!
	{
		if (data.commands[cmd_arming] > 1500)
		{
			double t_diff = chrono::duration<double, nano>(t_last - arming_t_started).count();

			if (data.isArming == false) // is arming
			{
				FileManagement::Log("Safety", "ARMING");
				data.isArming = true;
				arming_t_started = TimeM::now();
				led->setArming();

				data.status[status_armed] = 1;

				// going to level the drone
				imu->computeOffsets();
			}
			else if (data.isArming && t_diff * pow(10, -9) > 2 && data.isArmed == false)
			{
				FileManagement::Log("Safety", "Armed");
				data.isArmed = true;
				data.status[status_armed] = 2;
				led->setArmed();
			}
		}
		else
		{
			data.isArmed = false;
			data.isArming = false;
			led->backToPrevious();
			if(data.status[status_armed] != 0){
				FileManagement::Log("Safety", "Disarmed");
			}
			data.status[status_armed] = 0;
		}
	}
}

void setup()
{
	data = Data();
	// initialize log file
	FileManagement::initialize();
	FileManagement::Log("Main : Setup", "Version : " + version);
	FileManagement::Log("MAIN : main", "Frequency : " + to_string(frequency) + " Hz");

	// check if root
	if (getuid())
	{
		FileManagement::Log("Main : Setup", "Not root. Please launch it with root permission : sudo");
		throw "Not root";
	}

	if (check_apm()) // check if apm (ardupilot) is running
	{
		FileManagement::Log("Main", "APM is running ... could not run");
		throw "APM running";
	}

	// load data from json
	FileManagement::readDataSettings(data);

	led = std::make_shared<LEDManager>();
	led->setKO();

	imu = std::make_shared<IMU>();
	rc = std::make_shared<RCInputManager>();
	servo = std::make_shared<ServoManager>();
	stab = std::make_shared<Stabilisation>();
	sMana = std::make_shared<SensorManager>();
	remote = std::make_shared<Remote>();

	servo->initialize();
	//usleep(10000);
	servo->zero();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	stab->initialize(data.status);

	// start sensorManager
	sMana->startThread(data.status);

	// launch remote

	remote->launch(&data, &t);

	// set saving
	data.status[status_Saving] = 0;
	data.flag_not_ready = false; // now the quadcopter is ready !!!

	FileManagement::Log("Main : Setup", "Initializated");
	led->setOK();
	FileManagement::saveDataSettings(data);
}

void loop()
{
	// ************ TIME : get **********
	// get informations
	auto now = TimeM::now();

	// ************ IMU : update **********
	imu->setMode(data);
	imu->update(); // get imu data just after timing

	// ************ rc : read **********
	rc->read(data.commands);

	// ************ TIME : calculus : start **********
	// process informations

	// before start
	if (data.status[status_experience_mode] == 1)
	{
		data.status[status_experience_mode] = 2;
		FileManagement::Log("MAIN", "Experience launched");
		data.time_exp_start = TimeM::now();

		// we should save :
		data.orders[0] = 1;
	}

	double now_n = chrono::duration<double, nano>(now - time_start).count();

	double diff_nano = chrono::duration<double, nano>(now - t_last).count();
	// since last iteration

	if (data.status[status_experience_mode] == 2)
	{
		double time_exp_int = chrono::duration<double, nano>(now - data.time_exp_start).count();
		data.time_exp = time_exp_int * pow(10, -9) + exp_time_start;
		data.status[status_experience_time] = data.time_exp; // to display
	}

	t_last = now;
	// dt : convert nano to seconds
	float dt = diff_nano * pow(10, -9.0); // back in seconds
	t = now_n * pow(10, -9.0);			  // back in seconds
										  // back in seconds -2 because we start from past

	if (data.status[status_experience_mode] != 0 && data.time_exp > exp_time_end)
	{
		data.status[status_experience_mode] = 0;
		FileManagement::Log("MAIN", "Experience ended");
		data.orders[status_Saving] = 0;
		data.status[status_experience_time] = 0;
		data.orders[0] = 0; // stop saving
	}

	// ************ TIME : calculus : end **********

	// ************ update imu ************
	imu->setDt(dt); // update dt

	// ************ safety check ************
	safety();

	// ************ angles ************
	imu->getComplementar(data); // get angles complementar
	imu->getRates(data);
	imu->getAcceleration(data);

	// ************ stabilize ************
	stab->Stabilize(data, dt);

	// send output to motors
	servo->setDuty(data);
	i++;

	// ************ led display ************
	led->update();

	// ************ display frequency ************

	if (i % f_display_iterationscount == 0)
	{
		auto now = TimeM::now();
		float diff = chrono::duration<double, nano>(now - last).count();

		float f = f_display_iterationscount / (diff * pow(10, -9.0));
		cout << "[ MAIN ] : f : " << f << "Hz "
			 << "\n";
		last = now;
	}
	// end
}

// ************ Control Mode ************
auto last_call = TimeM::now();
bool useTimer = true;

void ModeFlight()
{
	double f_dt = 1 / frequency; //seconds
	long long int f_dt_n = f_dt * pow(10, 9);
	setup();

	// timer variables
	last_call = TimeM::now(); // reset timmer
	// loop
	while (true)
	{
		// waiting
		if (useTimer)
		{
			last_call += chrono::nanoseconds(f_dt_n);
			while (pow(10, -9) * chrono::duration<double, nano>(last_call - TimeM::now()).count() > 0)
			{
				// micro second
				// if error is superior to 10 micro second, reset clock
				if (pow(10, -3) * chrono::duration<double, nano>(TimeM::now() - last_call).count() > 9000 * f_dt)
				{
					FileManagement::Log("MAIN", "Clock reset : " + to_string(chrono::duration<double, nano>(TimeM::now() - last_call).count()) + " nanoseconds");
					// cout << pow(10, -3) * chrono::duration<double, nano>(TimeM::now() - last_call).count() << " " << 9000 *f_dt << "\n";
					last_call = TimeM::now();
				}
			}
		}

		// wait that we are at beggining

		// call loop
		loop();

		updateOrders();
		FileManagement::saveData(data, t);
	}
}

void InputMode()
{
	cout << "======================\n";
	cout << "Entering Input mode\n";
	cout << "======================\n";

	setup();

	// initialise ncurse
	initscr();
	//WINDOW *win = newwin(nlines, ncols, 0, 0);
	int start = 30;
	int length = 90;
	int divider = 1000 / length;
	while (true)
	{
		clear();
		rc->read(data.commands);
		for (int i = 0; i < cmd_size; i++)
		{
			std::string s = std::to_string(data.commands[i]);
			mvprintw(i, 1, s.c_str());

			// start and end
			mvprintw(i, start, "<");
			mvprintw(i, start + length, ">");

			mvprintw(i, start - 200 / divider, "|");
			mvprintw(i, start + length + 200 / divider, "|");

			if (data.commands[i] > 800)
			{
				mvprintw(i, (data.commands[i] - 1000) / divider + start, "x");
			}
			else
			{
				mvprintw(i, 10, "Err");
			}
		}
		refresh();
	}
}

void MotorControlMode()
{
	cout << "======================\n";
	cout << "Motor control mode\n";
	cout << "======================\n";

	setup();
	initscr();
	int pos = 0;

	mvprintw(0, 0, "Motor : 0");
	std::string drone = "2     0\n \\   / \n  |D|  \n /   \\ \n1     3\n";
	mvprintw(2, 0, drone.c_str());

	while (true)
	{

		int ch;
		nodelay(stdscr, TRUE);

		if ((ch = getch()) != ERR)
		{
			pos++;
			clear();
			std::string s = std::to_string(pos);

			mvprintw(0, 0, "Motor :");
			mvprintw(0, 8, s.c_str());
			mvprintw(2, 0, drone.c_str());
			refresh();

			if (pos > 3)
				pos = 0;
		}

		rc->read(data.commands);
		for (size_t i = 0; i < 4; i++)
		{
			data.motors_output[i] = 1000;
		}
		data.motors_output[pos] = data.commands[cmd_throttle];
		servo->setDuty(data);
	}
}

void HelpMode()
{
	cout << "======================\n";
	cout << "Entering Help mode\n";
	cout << "======================\n";
	cout << "Commands : \n";
	cout << "nothing : run the grogram\n";
	cout << "--inputs : display the inputs\n";
	cout << "--motors : Control the motors\n";
	cout << "--help : display the help\n";
}

// ************ MAIN ************

int main(int argc, char *argv[])
{
	// get command parmaters

	if (argc > 1 && strcmp(argv[1], "--inputs") == 0)
	{
		InputMode();
	}
	if (argc > 1 && strcmp(argv[1], "--motors") == 0)
	{
		MotorControlMode();
	}
	else if (argc > 1 && strcmp(argv[1], "--help") == 0)
	{
		HelpMode();
	}
	else
	{
		ModeFlight();
	}
	//Hz
}
