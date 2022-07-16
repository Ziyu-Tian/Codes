#include <stdio.h>
#include <math.h>
int main()
{
    int n;
    double h;
    double y, x,y_1,y_2;
    double result_odd = 0;
    double result_even = 0;
    double result = 0;
    printf("Please input the strips number n: \n");
    scanf("%d", &n);
    h = (2.0000 / n);
    y_1=pow((8*2+4),8.00/9.00);
    y_2=pow((8*4+4),8.00/9.00);
    for (int i = 2; i <= n; i++)
    {
        x = 2 + (i - 1) * h;
        y = pow(8*x+4,8.00/9.00);
        if (i % 2 == 0)
        {
            result_even += 4 * y;
        }
        else
        {
            result_odd += 2 * y;
        }
    }
    result = (y_1 + y_2 + result_odd + result_even) * (h / 3.000);
    printf("%.4lf", result);
    return 0;
}