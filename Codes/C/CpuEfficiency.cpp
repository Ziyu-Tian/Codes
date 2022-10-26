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
    ifstream file("/proc/stat");
    string line;
    string word;
    long long unsigned int sum = 0;
    float percentage[20][20];
    memset(percentage, 0, sizeof(percentage));

    if (!file.good())
    {
        cerr << "Could not open file.... exitting..." << endl;
        exit(EXIT_FAILURE);
    }

    clearScreen();
    cout<<"CPU\t"<<"busy\t        "<<"idle\t        "<<"system\t        "<<"nice\n";
    while (true)
    {
        int i = 0; 

        while (getline(file, line))
        {
            stringstream linestream(line);
            string token;
            getline(linestream, token, ' ');
            long long unsigned int user = 0;
            long long unsigned int nice = 0;
            long long unsigned int system = 0;
            long long unsigned int idle = 0;
            long long unsigned int iowait = 0;
            long long unsigned int irq = 0;
            long long unsigned int softirq = 0;
    
            if (token[0] == 'c' && token[1] == 'p' && !(token == "cpu"))
            {
                if (i > 3)
                {
                    break;
                }
                else
                {
                    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
                    sum = user + nice + system + idle + iowait + irq + softirq;
                    percentage[i][0] = (float)user * 100 / sum;
                    percentage[i][1] = (float)idle * 100 / sum;
                    percentage[i][2] = (float)system * 100 / sum;
                    percentage[i][3] = (float)nice * 100 / sum;
                   
                    cout <<"cpu"<<i<<"\t"
                         << fixed << setprecision(6)
                         << percentage[i][0] << "%\t"
                         << percentage[i][1] << "%\t"
                         << percentage[i][2] << "%\t"
                         << percentage[i][3] << "%" << endl;
                    ++i;
                }
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