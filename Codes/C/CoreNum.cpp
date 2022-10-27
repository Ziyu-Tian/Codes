#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>

using namespace std;

void clearScreen();

int CoreNum();

int main()
{
	cout<<CoreNum()<<endl;
	return 0;
}

int CoreNum()
{
	ifstream file("/proc/stat");
	string line;
	int num = 0;

	if (!file.good())
	{
		cerr << "Could not open file.... exitting..." << endl;
		exit(EXIT_FAILURE);
	}

	clearScreen();
	while (getline(file, line))
	{
		stringstream linestream(line);
		string token;
		getline(linestream, token, ' ');

		if (token[0] == 'c' && token[1] == 'p' && (token != "cpu" || num == 0))
		{

			num++;
		}
	}

	file.close();
	--num;
	/*
	cout << "------------------------------------------------------------------------"
		 << endl;
	cout <<"Total CPU Cores: "<< num << endl;
	*/
	return num;
}

void clearScreen()
{
	cout << "\033[2J\033[1;1H";
}
