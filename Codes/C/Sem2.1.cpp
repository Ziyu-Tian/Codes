
#include <iostream>
using namespace std;
int check_prime(int num);

int main()
{
    int number=0;
    cout << "Enter a number: ";
    cin >> number;
    if (check_prime(number) == 1)
    {
        cout << "Number " << number << " is not prime";
    }
    else
    {
        cout << "Number " << number << " is prime";
    }
    return 0;
}

int check_prime(int num) /* User-defined function to check prime number*/
{
    
    int j,flag = 0;
    
    for (j = 2; j <= num / 2; ++j)
    {
        if (num % j == 0)
        {
            flag = 1;
            break; 
        }
    }
    return flag;
}
