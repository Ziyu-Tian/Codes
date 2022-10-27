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


int CoreNum()
{
	ifstream file("/proc/stat");// input the "/proc/stat" file
	string line; // "line" stores the information of one line
	int num = 0; // "sum" is used to store the core number.

	// The open error warning

	if (!file.good())
	{
		cerr << "Could not open file.... exitting..." << endl;
		exit(EXIT_FAILURE);
	}
	// clean the error message
	clearScreen();

	// continue input line until the end of the file
	while (getline(file, line))
	{
		stringstream linestream(line);
		string token; // token to store the first word of line
		getline(linestream, token, ' ');// stop input when meeting space

		// add the num when token is "cpu"
		if (token[0] == 'c' && token[1] == 'p' )
		{
			num++;
		}
	}

	file.close();
	--num; // the cpu (total) should be excluded
	
	return num;
}

void clearScreen()
{
	cout << "\033[2J\033[1;1H";
}
// ANSI code, "\033" means "ESC" in octal. "[2j" is used to clean screen
// The function of clearScreen() is cleaning the current linux terminal
// and move the cursor to (1,1)