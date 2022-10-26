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
	string line ;
    int i =0;
    int j =0;
    long long unsigned int data[20][20];
    for (int m = 0; m < 20; m++)
    {
        for (int n = 0; n < 20; n++)
        {
            data[m][n] = 0;
        }
        
    }
    
	if (!file.good())
	{
		cerr << "Could not open file.... exitting..." << endl;
		exit(EXIT_FAILURE);
	}

	clearScreen();
	//while (true)
	//{

		while (getline(file, line))
		{
			stringstream linestream(line);
            stringstream stringsum;
			string token;
            string word;
            
			long long unsigned int user = 0;
			long long unsigned int nice = 0;
            long long unsigned int system = 0;
            long long unsigned int idle = 0;
            getline(linestream,token,' ');
            if (token[0] == 'c' && token[1] == 'p' && !(token == "cpu"))
            {
                while (linestream>>word)
                {
                    if (!(word[0] == 'c' && word[1] == 'p'))
                    {
                        stringsum << word;
                        stringsum >> data[i][j];
                        stringsum.clear();
                    
                    }
                    
                }
                
            }
            
            
		}
		//usleep(500000);
		//clearScreen();
		file.close();

		file.open("/proc/stat");
		if (!file.good())
		{
			cerr << "Could not open file.... exitting..." << endl;
			exit(EXIT_FAILURE);
		}
	//}
	file.close();
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            cout<<data[i][j]<<" ";
        }
        cout<<endl;
        
    }
    
	return 0;
}

void clearScreen()
{
	cout << "\033[2J\033[1;1H";
}