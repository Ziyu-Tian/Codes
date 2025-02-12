#include <stdio.h>
int sn(int n);
int main()
{
    int n;
    int sum=0;
    scanf("%d", &n);
    for (int i = 1; i <=n; i++)
    {
        sum+=sn(i);
    }
    
    printf("%d", sum);
    return 0;
}
int sn(int n)
{

    if (n == 1)
    {
        return 2;
    }
    else
    {
        return sn(n - 1) * 10 + 2;
    }
}