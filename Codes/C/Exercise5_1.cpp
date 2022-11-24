#include <iostream>
#include <bits/stdc++.h>
#include <cstdio>
using namespace std;
void lcd(int num...);
void lcd(int num...)
    va_list args;
    va_start(args, num);
    int array[100];
    memset(array, 0, 100);

    for (int i = 0; i < num; i++)
    {
        array[i] = va_arg(args, int);
    } 
    va_end(args);
    int max = *max_element(array,array+num);
    int sum=0;
    for ( ;;max++)
    {
        for (int j = 0;j<num; j++)
        {
            if (max%array[j]==0)
            {
                sum++;
            }
            else
            {
             sum = 0;
             break;   
            }
            
        }
        if (sum==num)
        {
            break;
        } 
    }
    cout << max << endl;

}

int main()
{
    lcd(3,3,4,5);
    return 0;
}

