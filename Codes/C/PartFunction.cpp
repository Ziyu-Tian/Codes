#include <stdio.h>
int main()
{
    int x;
    scanf("%d",&x);
    if (x<1)
    {
        printf("%d", x);
    }
    if (x>=1)
    {
        
        if (x>=10)
        { 
            printf ("%d",3*x-11);
        }
        else 
        printf("%d", 2 * x - 1);
    }
   
    return 0;
}