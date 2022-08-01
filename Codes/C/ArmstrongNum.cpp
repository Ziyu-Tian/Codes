#include <stdio.h>
#include <math.h>
#define RANGE 1000
int checkArmstrong(int n);
int main()
{
    for (size_t i = 100; i < RANGE; i++)
    {
        if (checkArmstrong(i) == 1)
        {
            printf("%zu\n", i);
        }
    }
    return 0;
}
int checkArmstrong(int n)
{
    int first, second, third;
    first = n / 100;
    second = (n / 10) % 10;
    third = n % 10;
    if (pow(first, 3) + pow(second, 3) + pow(third, 3) == n)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}