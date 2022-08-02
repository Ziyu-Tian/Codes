#include <stdio.h>
int PerfectNum(int n);
void FactorsPrint(int n);
int main()
{
    int n;
    scanf("%d", &n);

    if (PerfectNum(n) == 1)
    {
        FactorsPrint(n);
    }

    return 0;
}
int PerfectNum(int n)
{
    int sum = 0;
    for (size_t i = 1; i < n; i++)
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
void FactorsPrint(size_t n)
{
    printf("%zu its factors are ", n);
    for (size_t i = 1; i < n; i++)
    {
        if (n % 1 == 0)
        {
            (n == i) ? printf("%zu ", i) : printf("%zu\n", i);
        }
    }
}