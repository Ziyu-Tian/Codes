#include <stdio.h>
#include <math.h>
int length(int n);
int *Array(int *A, int n, int num);
int main()
{
    int num;
    int A[10];

    scanf("%d", &num);
    int *M = Array(A, length(num), num);
    printf("%d\n", length(num));
    for (int i = (length(num)-1); i >=0 ; i--)
    {

        if (i == 0)
        {
            printf("%d\n",M[i]);
        }
        else
        {
           printf("%d ", M[i]); 
        }       
        
    }
    for (int i = 0; i < length(num); i++)
    {
        printf("%d", A[i]);
    }

    return 0;
}
int length(int n)
{
    int digit = 0;
    while (n != 0)
    {
        n = n / 10;
        ++digit;
    }

    return (digit);
}
int *Array(int *A, int n, int num)
{

    for (int i = n - 1; i > 0; i--)
    {
        int temp = pow(10, n - 1);
        A[i] = num / temp;
        num = num % temp;
        --n;
    }
    A[0] = num;
    return A;
}
