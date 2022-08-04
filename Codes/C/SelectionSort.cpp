#include <stdio.h>
void SelectionSort(int array[], int length);
void SelectionSort(int array[], int length)
{
    /*
    int i, j;
    for (i = 0; i < length; i++)
    {
        int min = array[i];
        for (j = i; j < length; j++)//Firstly find the min num
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
    */
    for (int i = 0; i < length; i++)
    {
        int min = i;
        for (int j = i; j < length; j++)
        {
            if (array[j] < array[i])
            {
                min = j;
            }
        }
        int temp = array[i];
        array[i] = array[min];
        array[min] = temp;
    }
}
int main()
{
    int a[] = {3, 4, 9, 66, 5, 11, 2};
    int length = sizeof(a) / sizeof(int);
    SelectionSort(a, length);
    for (int i = 0; i < length; i++)
    {
        printf("%d ", a[i]);
    }
    return 0;
}