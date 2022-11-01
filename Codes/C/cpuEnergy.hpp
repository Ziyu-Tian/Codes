#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>
#include <iomanip>
#include "CoreNum.hpp"
using namespace std;

void energyPrint(void);


void energyPrint()
{
	ifstream time("/proc/uptime");
	string line;
	int Cores = CoreNum();

	if (!time.good())
	{
		cerr << "Could not open file.... exitting..." << endl;
		exit(EXIT_FAILURE);
	}

	//clearScreen();
	//while (true)
	//{

		while (getline(time, line))
		{
			stringstream linestream(line);
			string token;
			long double upTime = 0;

			long double idleTime = 0;

			long double ActivePower = 0;

			long double IdlePower = 0;

			linestream >> upTime >> idleTime;

			idleTime = idleTime / Cores;

			IdlePower = idleTime * 22 / 1000000;
			ActivePower = (upTime - idleTime) * 40 / 1000000;
			cout << "ENERGY\t"
				 << "In Activate State: "
				 << fixed << setprecision(2) << IdlePower << " MJoules" << endl
				 << "      \t"
				 << "In idle State: "
				 << ActivePower << " MJoules" << endl;
		}
		//usleep(500000);
		//clearScreen();
		time.close();

		time.open("/proc/uptime");
		if (!time.good())
		{
			cerr << "Could not open file.... exitting..." << endl;
			exit(EXIT_FAILURE);
		}
  time.close();
	}

	
