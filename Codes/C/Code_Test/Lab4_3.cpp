#include <stdio.h>
int main()
{

    int n;
    scanf("%d",&n);
    for (int i = 1; i <= n; i++)
    {
        printf("%d\t", i);
        printf("%d\t", 10 * i);
        printf("%d\t", 100 * i);
        printf("%d\n", 1000 * i);
    }
    return 0;
}