#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>
#include <string.h>
#include <iomanip>
using namespace std;
void clearScreen();

int main()
{
    ifstream file("/proc/meminfo");
    string line;

    if (!file.good())
    {
        cerr << "Could not open file.... exitting..." << endl;
        exit(EXIT_FAILURE);
    }
    clearScreen();
    while (true)
    {
        long double Total = 0;
        long double Free = 0;
        long double Buffers = 0;
        long double Cached = 0;
        while (getline(file, line))
        {
            stringstream linestream(line);
            string token;
            getline(linestream, token, ':');

            if (token == "MemTotal")
            {
                linestream >> Total;
                Total = Total / 1024;
            }
            if (token == "MemFree")
            {
                linestream >> Free;
                Free = Free / 1024;
            }
            if (token == "Buffers")
            {
                linestream >> Buffers;
                Buffers = Buffers / 1024;
            }
            if (token == "Cached")
            {
                linestream >> Cached;
                Cached = Cached / 1024;
            }
        }
        cout << "------------------------------------------------------------------------"
             << endl;
        cout << "MEMORY\t"
             << "Total: " << fixed << setprecision(2)
             << Total << "MB" << endl
             << "      \t"
             << "Free: " << Free << "MB" << endl
             << "      \t"
             << "Cached: " << Cached << "MB" << endl
             << "      \t"
             << "Buffers: " << Buffers << "MB" << endl;
        cout << "------------------------------------------------------------------------"
		 << endl;

        usleep(500000);
        clearScreen();
        file.close();

        file.open("/proc/meminfo");
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