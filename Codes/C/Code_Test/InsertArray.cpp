#include <stdio.h>
#include <string.h>
void SelectionSort(int array[], int length);
int main()
{
    int a[10];
    for (int i = 0; i < 9; i++)
    {
        scanf("%d", &a[i]);
    }
    scanf("%d", &a[9]);
    int len = sizeof(a) / sizeof(int);
    SelectionSort(a, len);
    for (int i = 0; i < 10; i++)
    {
        printf("%d\n", a[i]);
    }
    return 0;
}
void SelectionSort(int array[], int length)
{
    for (int i = 0; i < length; i++)
    {
        int min = i;
        for (int j = i; j < length; j++)
        {
            if (array[j] < array[min])
            {
                min = j;
            }
        }
        int temp = array[i];
        array[i] = array[min];
        array[min] = temp;
    }
}