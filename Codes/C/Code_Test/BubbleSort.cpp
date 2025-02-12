#include <stdio.h>
#include <string.h>
void sort(int array[],int length);
int main()
{
    int a[10];
    for (int i = 0; i < 10; i++)
    {
        scanf("%d",&a[i]);
    }
    
    int length = sizeof(a) / sizeof(int);
    sort(a,length);
    for (int i = 0; i < length; i++)
    {
        printf("%d\n",a[i]);
    }
    return 0;
}
void sort(int array[],int length)
{
    for (int i = 0; i < length; i++)
    {
        int temp;
        for (int j = i + 1; j < length; j++)
        {
            if (array[j] < array[i])
            {
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
}
