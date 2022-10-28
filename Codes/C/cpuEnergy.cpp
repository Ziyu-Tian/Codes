#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>
#include <iomanip>
using namespace std;

void clearScreen();

int main()
{
	ifstream file("/proc/uptime");
	string line;

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

			long double idleTime = 0;

			long double ActivePower = 0;

			long double IdlePower = 0;

			linestream >> upTime >> idleTime;

			idleTime = idleTime / 4;

			IdlePower = idleTime * 22 / 1000000;
			ActivePower = (upTime - idleTime) * 40 / 1000000;
			cout << fixed << setprecision(2) << IdlePower << " MJoules" << endl
				 << ActivePower << " MJoules" << endl;
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
	return 0;
}

void clearScreen()
{
	cout << "\033[2J\033[1;1H";
}
