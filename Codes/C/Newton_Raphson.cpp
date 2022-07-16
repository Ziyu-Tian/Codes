#include<stdio.h>
#include<math.h>
int main()
{
    double x;
    int n;
    printf("Please input the start x_0:\n");
    scanf("%lf",&x);
    printf("Please input the n:\n");
    scanf("%d",&n);
    printf("n\tf(x)\tf'(x)\tx_n\n");
    for (int i = 0; i <=n; i++)
    {
        double f_1,f_2;
        f_1=0.3*sin(x/10)+0.4*sin(2*x/10)-0.2;
        f_2=0.03*cos(x/10)+0.08*cos(x/5);
        printf("%d\t%.4lf\t%.4lf\t%.4lf\n",i,f_1,f_2,x);
        x=x-(f_1/f_2);
    }
    
    return 0;
}