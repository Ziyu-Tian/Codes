#include <stdio.h>
#include <string.h>
void invert(int array[],int length);
void invert(int array[],int length)
{
    
    for (int i = length - 1; i >= 0; --i)
    {
        printf("%d ", array[i]);
    }
}
int main()
{
    int a[10];
    int length = sizeof(a) / sizeof(int);
    for (int i = 0; i < 10; i++)
    {
        scanf("%d", &a[i]);
    }
    invert(a,length);
    return 0;
}