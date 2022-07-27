#include <stdio.h>
int recursive(int n);
int main()
{
    int a;
    while (scanf("%d", &a) != EOF)
    {
        if (a!=0)
        {
        printf("%d\n",recursive(a));
        }
    }
    
    return 0;
}
int recursive(int n)
{
    if (n <=4)
    {
        return n;
    }
    return recursive(n - 3) + recursive(n - 1);
}