#include<stdio.h>
int gcd(int m,int n);
int main()
{
    int a,b;
    scanf("%d %d",&a,&b);
    printf("%d ",gcd(a,b));
    printf("%d",a*b/gcd(a,b));
    return 0;
}
int gcd(int m,int n)
{
    if (m<n)
    {
        int temp;
        temp=m;
        m=n;
        n=temp;
    }
    if (m%n!=0)
    {
        return gcd(n,m%n);
    }
    else
    {
        return n;
    }
}
