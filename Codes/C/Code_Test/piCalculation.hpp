#include<bits/stdc++.h>
using namespace std;
double Pi(double n);
double Pi(double n)
{
    if (n==1)
    {
        return 4;
    }
    else
    {
        return Pi(n-1)+(4/(2*n-1))*pow(-1,n-1);
    }
    
}