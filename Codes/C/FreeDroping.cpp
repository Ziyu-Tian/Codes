#include<stdio.h>
#include<math.h>
float FreeDrop(int m,int n);
float TotalDistance(int m,int n);
int main()
{
    int m,n;
    scanf("%d %d",&m,&n);
    printf("%.2f %.2f",FreeDrop(m,n),TotalDistance(m,n));
    return 0;
}
float FreeDrop(int m,int n)
{
    return m*pow(0.5,n);
}
float TotalDistance(int m,int n)
{
    float sum=0;
    for (int i = 1; i <=n; i++)
    {
        if (i==1)
        {
            sum+=m;
        }
        else
        {
            sum+=2*m*pow(0.5,i-1);
        }
    }
    return sum;
}
