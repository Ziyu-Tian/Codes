#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>

using namespace std;

void clearScreen();

int main () {
    ifstream file("/proc/stat");
    string line;
	int num =0;

    if(!file.good()){
	cerr << "Could not open file.... exitting..." << endl;
	exit(EXIT_FAILURE);
    }

    clearScreen();
    while(true){
	while(getline(file, line)){
	    stringstream linestream(line);
	    string token;
	    getline(linestream, token, ' ');
	
	    if(token[0] == 'c' && token[1]=='p'){
			
	        num++;
	    }
	}
	cout<<num<<endl;
	//usleep(500000);
	//clearScreen();
        file.close();

        file.open("/proc/stat");

        if(!file.good()){
            cerr << "Could not open file.... exitting..." << endl;
            exit(EXIT_FAILURE);
        }
    }

    file.close();
    return 0;
}


void clearScreen(){
    cout << "\033[2J\033[1;1H";
}

