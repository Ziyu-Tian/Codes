#include <stdio.h>
int main()
{
    int a = 0;
    int num = 0;
    printf("Please input your number:");
    scanf("%d", &num);
    for (int i = 2; i < num; i++)
    {
        if (num % i == 0)
        {
            a++;
        }
    }
    if (a == 0)
    {
        printf("%d is a prime number.\n", num);
    }
    else
    {
        printf("%d is not a prime number.\n", num);
    }
    return 0;
}