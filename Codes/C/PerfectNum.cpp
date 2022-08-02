#include <stdio.h>
int PerfectNum(int n);
void FactorsPrint(int n);
int main()
{
    int n;
    scanf("%d", &n);
    for (int i = 1; i < n; i++)
    {
        if (PerfectNum(i) == 1)
        {
            FactorsPrint(i);
        }
    }
    return 0;
}
int PerfectNum(int n)
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
void FactorsPrint(int n)
{
    printf("%d its factors are ", n);
    for (int i = 1; i < n; i++)
    {
        if (n % i == 0)
        {
            printf("%d ", i);
        }
    }
    printf("\n");
}
