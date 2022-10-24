#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
using namespace std;
int CoreNum(void);
int main()
{
    cout<<CoreNum()<<endl;
    usleep(500000);
    return 0;
}
int CoreNum(void)
{
    ifstream file("/proc/stat");
    string line;
    int core = 0;
    if (!file.good())
    {
        cerr << "Open Error! " << endl;
        exit(0);
    }
    while (true)
    {
        while (getline(file, line))
        {
            stringstream linestream(line);
            string token;
	    getline(linestream, token, ' ');
	
	    if(token[0]== 'c'&& token[1]=='p'){
	        ++core;
        }
    }
    file.close();
    return core;
}
