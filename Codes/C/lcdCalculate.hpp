#include<bits/stdc++.h>
using namespace std;
void lcd(int num...);
// lcd using va_list function, the 'num' is the number of parameters
int gcd(int v1, int v2);
// gcd using Euclidean Algorithm

template <typename... Arguments>
int lcd_2(int val1, Arguments... values)
{
return val1 * lcd_2(values...) / gcd(val1,lcd_2(values...));

// the lcd(a,b) = a*b/gcd(a,b)

}
template<>
int lcd_2(int d)
{
return d;
}

//--------- 

void lcd(int num...)
{
    va_list args; // va_list means the function begin
    va_start(args, num); // num is the number of the parameters
    int array[100]; 
    memset(array, 0, 100); // initialize the array with '0'

    for (int i = 0; i < num; i++)
    {
        array[i] = va_arg(args, int); 
        // va_arg means visit the next int variable
    } 
    va_end(args);// the function ends
    int max = *max_element(array,array+num);// max_elements return the max 
    of the address of the max element in array.
    int sum=0;
    for ( ;;max++) // search the lcd from max element reduce the try times
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
    if (m<n) // justify the larger one and smaller one
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

