#include <iostream>
#include <string>
#include <sstream> 
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
    int low, high, i, flag;

    string low_string, high_string;
    stringstream string_stream;

   //Managing the arguments
   if(argc < 5){
   	cerr << "USAGE: " << argv[0] << " -l LOWER_RANGE -h HIGHER_RANGE" << endl;
   	cerr << "-l is followed by lower range number, -h is followed by higher range number" << endl;
   	exit(1);
   }
   
   //Processing the first pair of arguments and sending error messages
   low_string = string(argv[1]);
   if(low_string == "-l"){
   //C++ stringstream is used to create a stream like console for converting from string to number, but this time only for strings
	string_stream << argv[2];
   	string_stream >> low;
   	if(low < 0){
   		cerr << "Lower range less than zero, nothing to generate, exitting...." << endl;
   		exit(1);
   	}
   }

   //Clearing the stringstream
   string_stream.clear();
   
   high_string = string(argv[3]);
   if(high_string == "-h"){
   //C++ stringstream is used to create a stream like console for converting from string to number, but this time only for strings
	string_stream << argv[4];
   	string_stream >> high;
   	if(high < 0 || high < low){
   		cerr << "Higher range lower than zero/low, nothing to search for in the string, exitting...." << endl;
   		exit(1);
   	}
   }

    cout << "Prime numbers between " << low << " and " << high << " are: ";

    while (low < high)
    {
        flag = 0;

        for(i = 2; i <= low/2; ++i)
        {
            if(low % i == 0)
            {
                flag = 1;
                break;
            }
        }

        if (flag == 0)
            cout << low << " ";

        ++low;
    }

    return 0;
}


