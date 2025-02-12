#include <stdio.h>
#include <math.h>
int main()
{
    int n;
    double h, y, x, y_1, y_2;
    double middle = 0;
    double result = 0;
    printf("Please input the strip number n:\n");
    scanf("%d", &n);
    h = 2.000 / n;
    y_1=pow((8*2+4),8.00/9.00);
    y_2=pow((8*4+4),8.00/9.00);
    for (int i = 2; i <= n; i++)
    {
        x = 2 + (i - 1) * h;
        y = pow((8 * x + 4), 8.00 / 9.00);
        middle += y;
    }
    result = (h / 2) * (y_1 + (middle * 2)+y_2);
    printf("%.4lf", result);
    return 0;
}