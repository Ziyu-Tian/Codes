#include <iostream>
#include <string>
#include <fstream>
using namespace std;
int fib(int n);
int main()
{
    ofstream write; // define the stream 
    write.open("fish.txt");
    int n; // setting the number of lines
    cout << "Please input the number:" << endl;
    cin >> n;
    for (int i = 0; i < n+1; i++)
    {
        int num = fib(i);
        for (int j = 0; j < num; j++)
        {
            write<<"><((º> "; // write the fish into file "write"
        }
        write<<endl;
    }
    write.close();
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