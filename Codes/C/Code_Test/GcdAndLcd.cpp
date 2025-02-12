#include<stdio.h>
int gcd(int m,int n);
int lcd(int m,int n);
int main()
{
    int m,n;
    scanf("%d %d",&m,&n);
    printf("%d %d",gcd(m,n),lcd(m,n));
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
int lcd(int m,int n)
{
    return m*n/gcd(m,n);
}

