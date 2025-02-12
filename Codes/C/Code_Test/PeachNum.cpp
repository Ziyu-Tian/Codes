#include<stdio.h>
int PeachFind(int n);
int PeachFind(int n)
{
    if (n==1)
    {
        return 1;
    }
    else
    {
        return 2*(PeachFind(n-1)+1);
    } 
}
int main()
{
    int n;
    scanf("%d",&n);
    printf("%d",PeachFind(n));
    return 0;
}