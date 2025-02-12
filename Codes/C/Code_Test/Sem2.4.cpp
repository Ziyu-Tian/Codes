#include<iostream>
#include<time.h>
using namespace std;
void new_func1(void);
void func1(void);
static void func2(void);
int main(void)
{
    cout << endl << " Executing main function" << endl;
    unsigned int i = 0;
    for(;i<0xffffff;i++);
    func1();
    func2();
    return 0;
}
void new_func1(void)
{   
    clock_t startTime,endTime;
    startTime = clock();
    cout << endl << " Executing new_func1" << endl;
    unsigned int i = 0;
    for(;i<0xffffffee;i++);
    endTime = clock();
    cout << "new_fun1 time: " << (double)(endTime - startTime)/CLOCKS_PER_SEC << endl;
    return;
}
void func1(void)
{
    clock_t startTime,endTime;
    startTime = clock();
    cout << endl << " Executing function func1" << endl;
    unsigned int i = 0;
    for(;i<0xffffffff;i++);
    new_func1();
    endTime = clock();
    cout << "func1 time: " << (double)(endTime - startTime)/CLOCKS_PER_SEC << endl;
    return;
}
static void func2(void)
{
    clock_t startTime,endTime;
    startTime = clock();
    cout << endl << " Executing func2" << endl;
    unsigned int i = 0;
    for(;i<0xffffffaa;i++);
    endTime = clock();
    cout << "func2 time: " << (double)(endTime - startTime)/CLOCKS_PER_SEC<< endl;
    return;
}