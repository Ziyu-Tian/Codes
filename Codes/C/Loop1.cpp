#include <iostream>
using namespace std;
void SelectionSort(int array[], int length);
int main()
{
    int a[10];
    for (int i = 0; i < 10; i++)
    {
        //scanf("%d", &a[i]);
        cin>>a[i];
    }
    int length = sizeof(a) / sizeof(int);
    SelectionSort(a, length);
    for (int i = 0; i < length; i++)
    {
        //printf("%d\n", a[i]);
        cout<<a[i]<<endl;
        
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