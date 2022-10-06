#include<iostream>
#include<ctime>
using namespace std;
int main()
{
    time_t t = time(0);
    char name[100];
    tm tt = *localtime(&t);
    printf("Please input your name:\n");
    scanf("%s",name);
    (tt.tm_hour < 12)?printf("Good morning, %s",name) : printf("Good Afternoon, %s",name);
    return 0;
}