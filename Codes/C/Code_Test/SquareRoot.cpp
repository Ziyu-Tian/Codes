#include<stdio.h>
#include<math.h>
float squareroot(float x)
{
    float sqt_1=1;
    float sqt_2=0;
    float a=x;  
    while (fabs(sqt_1-x)>0.00001)
    {
        sqt_1= (x+a/x)/2;
        sqt_2= (sqt_1+a/sqt_1)/2; 
        x=sqt_2;
    }
    return sqt_2;

}
int main()
{
    float a;
    scanf("%f",&a);
    printf("%.3f",squareroot(a));
    return 0;
}