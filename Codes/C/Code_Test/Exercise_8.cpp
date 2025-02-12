#include <iostream>
#include <string>
using namespace std;
int fib(int n);
int main()
{
    int n; // setting the number of lines
    cout << "Please input the number:" << endl;
    cin >> n;
    for (int i = 0; i < n+1; i++)
    {
        int num = fib(i);
        for (int j = 0; j < num; j++)
        {
            cout<<"><((º> "; // print the fish 
        }
        cout<<endl;
    }
    
    return 0;
}

int fib(int n)// The Fib function 
{
    if (n == 1)
    {
        return 1;
    }
    else if (n == 2)
    {
        return 1;
    }
    else if (n == 0)
    {
        return 0;
    }
    else
    {
        return fib(n - 1) + fib(n - 2);
    }
}