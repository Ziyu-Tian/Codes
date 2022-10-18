#include <iostream>
#include <string>
#include <fstream>
#include<cstring>
#include<sstream>
using namespace std;
int main()
{
    ifstream ecg;
    ecg.open("data.csv");
    int Range[11];
    memset(Range,0,11);
    // the array need to be set '0'
    int a; // a is the data
    int sign;
    int temp;
    while (!ecg.eof())
    {
        ecg >> a;
        if (a >= 0 && a <= 100)
            sign = 0;
        if (a >= 101 && a <= 200)
            sign = 1;
        if (a >= 201 && a <= 300)
            sign = 2;
        if (a >= 301 && a <= 400)
            sign = 3;
        if (a >= 401 && a <= 500)
            sign = 4;
        if (a >= 501 && a <= 600)
            sign = 5;
        if (a >= 601 && a <= 700)
            sign = 6;
        if (a >= 701 && a <= 800)
            sign = 7;
        if (a >= 801 && a <= 900)
            sign = 8;
        if (a >= 901 && a <= 1000)
            sign = 9;
        if (a >= 1001 && a <= 1023)
            sign = 10;
        switch (sign)
        {
        case 0:
        {
            temp = Range[0];
            Range[0] = (temp + 1);
            break;
        }
        case 1:
        {
            temp = Range[1];
            Range[1] = (temp + 1);
            break;
        }
        case 2:
        {
            temp = Range[2];
            Range[2] = (temp + 1);
            break;
        }
        case 3:
        {
            temp = Range[3];
            Range[3] = (temp + 1);
            break;
        }
        case 4:
        {
            temp = Range[4];
            Range[4] = (temp + 1);
            break;
        }
        case 5:
        {
            temp = Range[5];
            Range[5] = (temp + 1);
            break;
        }
        case 6:
        {
            temp = Range[6];
            Range[6] = (temp + 1);
            break;
        }
        case 7:
        {
            temp = Range[7];
            Range[7] = (temp + 1);
            break;
        }
        case 8:
        {
            temp = Range[8];
            Range[8] = (temp + 1);
            break;
        }
        case 9:
        {
            temp = Range[9];
            Range[9] = (temp + 1);
            break;
        }
        case 10:
        {
            temp = Range[10];
            Range[10] =(temp+1);
            break;
        }
        }
    }
    ecg.close();
    for (int i = 0; i < 11; i++)
    {
        int change = Range[i];
        if (Range[i]>205)
        {
            Range[i] = (change/205);
        }
        else
        {
            Range[i] = 0;
        }
    }
    for (int i = 0; i < 11; i++)
    {
        cout<<"B"<<i<<" "<<"|";
        for (int j = 0; j <Range[i]; j++)
        {
            cout<<"-";
        }
        cout<<endl;
    }
    for (int i = 0; i < 11; i++)
    {
        cout<<Range[i]<<endl;
    }
    
    
    return 0;
}
