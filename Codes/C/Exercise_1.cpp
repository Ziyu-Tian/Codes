#include<iostream>
#include<ctime> // ctime is the library needed for system time
using namespace std;
int main()
{
    time_t t = time(0); // initialize the 't' to the system time
    char name[100]; // the character array to store the user's name
    tm tt = *localtime(&t); // define a new structure 'tt' with tm type
    // localtime() is used to change the UTC to user's time 
    printf("Please input your name:\n");    
    scanf("%s",name); // Input the name
    // the internal struct "tm" contain the following members:
    // tm_sec, tm_min, tm_hour, etc. All these members are int type.
    (tt.tm_hour < 12)?printf("Good morning, %s",name) : printf("Good Afternoon, %s",name);
    // the hour larger than 12 will be morning and below will be afternoon
    // the ternary operator can use for small if-else 
    return 0;
}



