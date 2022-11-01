#include "stat.hpp"

void clearScreen()
{
	cout << "\033[2J\033[1;1H";
}
// ANSI code, "\033" means "ESC" in octal. "[2j" is used to clean screen
// The function of clearScreen() is cleaning the current linux terminal
// and move the cursor to (1,1)

//--------------------------------------------

int CoreNum()
{
	ifstream file("/proc/stat"); // input the "/proc/stat" file
	string line;				 // "line" stores the information of one line
	int num = 0;				 // "sum" is used to store the core number.

	// The open error warning

	if (!file.good())
	{
		cerr << "Could not open file.... exitting..." << endl;
		exit(EXIT_FAILURE);
	}
	// clean the error message
	//clearScreen();

	// continue input line until the end of the file
	while (getline(file, line))
	{
		stringstream linestream(line);
		string token;					 // token to store the first word of line
		getline(linestream, token, ' '); // stop input when meeting space

		// add the num when token is "cpu"
		if (token[0] == 'c' && token[1] == 'p')
		{
			num++;
		}
	}

	file.close();
	--num; // the cpu (total) should be excluded

	return num;
}
//------------------------------------------------

void cpuPrint()
{
    ifstream stat("/proc/stat");
    string line;
    string word;
    long long unsigned int sum = 0;
    float percentage[200][200];                // two-dimension array store the table of cpu
    memset(percentage, 0, sizeof(percentage)); // initialize array with 0
    int Cores = CoreNum();
    if (!stat.good())
    {
        cerr << "Could not open file.... exitting..." << endl;
        exit(EXIT_FAILURE);
    }

    //clearScreen();

    // while (true)
    // {
    int i = 0;

    cout << "-----------------------------------------------------------------------";
    cout << endl;
    cout << "Total CPU Cores: " << Cores << endl
         << "-----------------------------------------------------------------------"
         << endl 
         << "CPU \t" 
         << "busy \t"
         << "idle \t"
         << "system \t"
         << "nice"<<endl;
    while (getline(stat, line))
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
            if (i > (Cores - 1)) // The number of the core is '4', but the index is from '0'
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

                cout << "cpu" << i << "\t"
                     << fixed << setprecision(1)
                     << percentage[i][0] << "%\t"
                     << percentage[i][1] << "%\t"
                     << percentage[i][2] << "%\t"
                     << percentage[i][3] << "%" << endl;
                ++i;
            }
        }
    }

     stat.close();
     stat.open("/proc/stat");
    
    if (!stat.good())
    {
        cerr << "Could not open file.... exitting..." << endl;
        exit(EXIT_FAILURE);
    }
    

     stat.close();
}