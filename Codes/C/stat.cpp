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
    ifstream file("/proc/stat");
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
            long long unsigned int user_cycles;
            long long unsigned int nice_cycles;
            long long unsigned int system_cycles;
            long long unsigned int idle_cycles;
            getline(linestream, token, ' ');
            if (token == "cpu")
            {
                linestream >> user_cycles >> nice_cycles >> system_cycles >>
                    idle_cycles;
                cout << user_cycles << "\t" << nice_cycles << "\t" << system_cycles << "\t" << idle_cycles << endl;
            }
        }
        usleep(500000);
        clearScreen();
        file.close();
        file.open("/proc/stat");
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
