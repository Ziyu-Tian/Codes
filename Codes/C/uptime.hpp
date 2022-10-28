#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>
#include "CoreNum.hpp"
using namespace std;

void dataPrint(long double uptime, long double idletime);


void timePrint(void);


void timePrint()
{
	ifstream file("/proc/uptime");
	string line;
	int Cores = CoreNum();

	if (!file.good())
	{
		cerr << "Could not open file.... exitting..." << endl;
		exit(EXIT_FAILURE);
	}

	clearScreen();
	while (true)
	{

		while (getline(file, line))
		{
			stringstream linestream(line);
			string token;
			long double upTime = 0;
			// cout<<line<<endl;
			long double idleTime = 0;

			linestream >> upTime >> idleTime;
			idleTime = idleTime / Cores;
			// cout << upTime << "\t" << idleTime << endl;
			dataPrint(upTime, idleTime);
		}
		usleep(500000);
		clearScreen();
		file.close();

		file.open("/proc/uptime");
		if (!file.good())
		{
			cerr << "Could not open file.... exitting..." << endl;
			exit(EXIT_FAILURE);
		}
	}
	file.close();
}

void dataPrint(long double uptime, long double idletime)
{

	int hour, minute, second, total = 0;
	int hour_1, minute_1, second_1, total_1 = 0;
	stringstream s;
	stringstream s_1;
	s << uptime;
	s >> total;
	second = total % 60;
	minute = total / 60;
	hour = minute / 60;
	minute = minute % 60;
	s_1 << idletime;
	s_1 >> total_1;
	second_1 = total_1 % 60;
	minute_1 = total_1 / 60;
	hour_1 = minute_1 / 60;
	minute_1 = minute_1 % 60;
	cout << "------------------------------------------------------------------------"
		 << endl;
	cout << "SYSTEM"
		 << "\t";
	printf("UP for %d hours %d minutes and %d seconds\n", hour, minute, second);
	printf("      \t");
	printf("IDLE for %d hours %d minutes and %d seconds\n", hour_1, minute_1, second_1);
	// cout<<idletime<<endl;
}
