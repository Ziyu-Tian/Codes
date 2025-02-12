#include <stdio.h>
#include <math.h>
int sum_1(int n);
int sum_2(int n);
float sum_3(float n);
int main()
{
    int a,b;
    float c;
    scanf("%d %d %f",&a,&b,&c);
    printf("%.2f",sum_1(a)+sum_2(b)+sum_3(c));
    return 0;
}
int sum_1(int n)
{
    int s_1 = 0;
    for (int i = 1; i <= n; i++)
    {
        s_1 += i;
    }
    return s_1;
}
int sum_2(int n)
{
    int s_2 = 0;
    for (int i = 1; i <=n; i++)
    {
        s_2 += pow(i, 2);
    }
    return s_2;
}
float sum_3(float n)
{
    float s_3 = 0;
    for (float i = 1; i <=n; i++)
    {
        s_3 += (1 /i);
    }
    return s_3;
}
