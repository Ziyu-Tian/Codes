#include <stdio.h>
#include <cmath>
int max(int x, int y)
{
    return (x > y ? x : y);
}
int main()
{
    int a[5];
    for (int i = 0; i < 3; i++)
    {
        scanf("%d", &a[i]);
    }
    int m=max(a[0],a[1]);
    m= max(m,a[2]);
    printf("%d",m);
    return 0;
}