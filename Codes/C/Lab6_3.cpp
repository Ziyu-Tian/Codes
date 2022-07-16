#include <stdio.h>
int sum(int a[], int length);
int sum(int a[], int length)
{
    int ans = 0;
    for (int i = 0;i<length; i++)
    {
        ans += a[i];
    }
    return ans;
}
int main()
{
    int a[1000];
    int number;
    printf("Input the number of elements to be stored in the array : \n");
    scanf("%d", &number);
    printf("Input %d elements in the array: \n",number);
    for (int i = 0; i < number; i++)
    {
        scanf("%d", &a[i]);
    }
    for (int i = 0; i < number; i++)
    {
        printf("element - %d : %d\n", i, a[i]);
    }
    printf("Sum of all elements stored in the array is : %d", sum(a,number));
    return 0;
}