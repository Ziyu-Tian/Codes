#include<iostream>
#include<cmath> // cmath is needed for sqrt()
using namespace std;

int main()
{
    int num;
    float sqr;
    cout<<"Enter a number:"<<endl; // enter the number
    cin>>num;
    sqr = sqrt(num); // use the sqrt with type 'double' to store the square root
    if (num == sqr * sqr)// If the square result is equal to itself
    {
        printf("The number has an integer root of %.0f.\n",sqr);
        // print the sqrt
    }
    else
    {
        printf("The number does not have an integer root.\n");
    }

    return 0;

}