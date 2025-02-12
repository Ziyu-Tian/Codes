#include <stdio.h>
double square(double n);
double square(double n)
{
   double result;
    result = n * n;
    return result;
}
int main()
{
    double a, ans;
    printf("Please input your initial number: ");
    scanf("%lf", &a);
    ans = square(a);
    printf("The square of %.2lf is %.2lf", a,ans);
    return 0;
}