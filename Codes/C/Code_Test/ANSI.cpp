#include<iostream>
using namespace std;
void gotoxy(int x,int y);
int main()
{
    gotoxy(0,3);
    cout<<"Hello,world"<<endl;
    return 0;
}
void gotoxy(int x, int y) {
    printf("%c[%d;%df", 0x1b, y, x);
    // printf("%c[%d;%dH", 0x1b, y, x);
}
