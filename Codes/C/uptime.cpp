#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>
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
            double upTime;
            double idleTime;

			//getline(linestream, token, ' ');
            linestream>>upTime>>idleTime;
            cout<<upTime<<"\t"<<idleTime<<endl;
		}

		 usleep(50000);
		 clearScreen();
	}

	file.close();

	return 0;
}

void clearScreen()
{
	cout << "\033[2J\033[1;1H";
}
