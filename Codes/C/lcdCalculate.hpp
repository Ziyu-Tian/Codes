#include<bits/stdc++.h>
using namespace std;
void lcd(int num...);

int gcd(int v1, int v2);

template <typename... Arguments>
int lcd_2(int val1, Arguments... values)
{
return val1 * lcd_2(values...) / gcd(val1,lcd_2(values...));
}
template<>
int lcd_2(int d)
{
return d;
}

//--------- 

void lcd(int num...)
{
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

int gcd(int m,int n)
{
    if (m<n)
    {
        int temp;
        temp=m;
        m=n;
        n=temp;
    }
    if (m%n!=0)
    {
        return gcd(n,m%n);
    }
    else
    {
        return n;
    }
}

