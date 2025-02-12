#include <stdio.h>
int findmax(int a[], int length)
{
    int max;
    for (int i = 0; i < length; i++)
    {
        for (int j = i+1; j < length; j++)
        {
            if (a[i]>a[j])
            {
                max=a[i];
                a[j]=max;
            }
            else{
                max=a[j];
                a[j]=max;
            }
        }
    }
    return max;
}
int main()
{
    int a[1000];
    int number;
    printf("Input the number of elements to be stored in the array : \n");
    scanf("%d", &number);
    printf("Input %d elements in the array: \n", number);
    for (int i = 0; i < number; i++)
    {
        scanf("%d", &a[i]);
    }
    for (int i = 0; i < number; i++)
    {
        printf("element - %d : %d\n", i, a[i]);
    }
    printf("Maximum element is : %d", findmax(a, number));
    return 0;
}