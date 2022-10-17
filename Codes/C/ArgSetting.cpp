#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int main(int argc, char *argv[])
{
    string low_string, high_string, file_name;
    int low, high, n;
    stringstream string_stream;

    if (argc < 5)
    {
        cout<<"The file address is "<<argv[0]<<endl;
        exit(1);
    }
    low_string = string(argv[1]);
    high_string = string(argv[3]);
    if (low_string == "-f")
    {
        string_stream << argv[2];
        string_stream >> file_name;
    }
    string_stream.clear();
    if (high_string == "-n")
    {
        string_stream << argv[4];
        string_stream >> n;
    }

    ofstream openFile;
    openFile.open(file_name);
    int sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum = sum +i;
    }
    
    openFile << sum << endl;

    openFile.flush();
    openFile.close();

    return 0;
    

    
}