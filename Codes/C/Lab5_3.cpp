#include <stdio.h>
#include <math.h>
int checkArmstrong(int n);
int checkPerfect(int n);
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
int checkPerfect(int n)
{
    int sum = 0;
    for (int i = 1; i < n; i++)
    {
        if (n % i == 0)
        {
            sum += i;
        }
    }
    if (sum == n)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int main()
{
    int n;
    printf("Please input the number: ");
    scanf("%d", &n);
    if (checkArmstrong(n) == 1)
    {
        printf("The %d is an armstrong number.\n", n);
    }
    else
    {
        printf("The %d is not an armstrong number.\n", n);
    }
    if (checkPerfect(n) == 1)
    {
        printf("The %d is the perfect number.\n",n);
    }
    else
    {
        printf("The %d is not the perfect number.\n",n);
    }

    return 0;
}