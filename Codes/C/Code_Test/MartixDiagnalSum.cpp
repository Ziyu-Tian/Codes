#include<stdio.h>
void DiagonalSum(int array[3][3]);
void DiagonalSum(int array[3][3])
{
    int Sum1 = 0;
    int Sum2 = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i==j)
            {
                Sum1+=array[i][j];
            }
        }
    }
    Sum2+=array[2][0]+array[0][2]+array[1][1];
    printf("%d %d",Sum1,Sum2);
}
int main()
{
    int a[3][3];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            scanf("%d",&a[i][j]);
        }
    }
    DiagonalSum(a);
    return 0;
}