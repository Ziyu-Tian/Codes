#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <unistd.h>
using namespace std;
void dataPrint(long double uptime, long double idletime);
int main()
{
    dataPrint(30000,36000);
    
    
    return 0;
}
void dataPrint(long double uptime, long double idletime)
{

	int hour, minute, second, total = 0;
	int hour_1, minute_1, second_1, total_1 = 0;
	stringstream s;
	s << uptime;
	s >> total;
	s.clear();
	second = total % 60;
	minute = total / 60;
	hour = minute / 60;
	minute = minute % 60;
	s << idletime;
	s >> total_1;
	second_1 = total_1 % 60;
	minute_1 = total_1 / 60;
	hour_1 = minute_1 / 60;
	minute_1 = minute_1 % 60;
	cout << "---------------------------------------------------------------------------------------------"
		 << endl;
	cout << "SYSTEM"
		 << "\t";
	printf("UP for %d hours %d minutes and %d seconds\n", hour, minute, second);
	printf("      \t");
	printf("IDLE for %d hours %d minutes and %d seconds\n", hour_1, minute_1, second_1);
	//cout<<idletime<<endl;
}