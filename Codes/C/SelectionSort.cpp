#include<stdio.h>
void SelectionSort(int array[],int length);
int main()
{
    int a[10];
    for (int i = 0; i < 10; i++)
    {
        scanf("%d",&a[i]);
    }
    int length = sizeof(a) / sizeof(int);
    SelectionSort(a, length);
    for (int i = 0; i < length; i++)
    {
        printf("%d\n", a[i]);
    }
    return 0;
}
void SelectionSort(int array[], int length)
{
    
    for (int i = 0; i < length; i++)
    {
        int min = array[i];
        for (int j = i; j < length; j++)//Firstly find the min num
        {
            if (array[j] < min)
            {
                int temp = array[j];
                array[j] = min;
                min = temp;
            }
        }
        array[i]=min;
    }
 
}