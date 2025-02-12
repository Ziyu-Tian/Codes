#include <stdio.h>
int main()
{
    int a, b;
    scanf("%d %d", &a, &b);
    if (a == b)
    {
        printf("These numbers are equal.\n");
    }
    else
    {
        if (a > b)
        {
            printf("%d is the largest.\n", a);
        }
        else
            printf("%d is the largest.\n", b);
    }
    return 0;
}