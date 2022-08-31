#include<stdio.h>
#include<math.h>
int main() {
    double a, b, c;
    scanf("%lf %lf %lf", &a, &b, &c);
    double delta, solution1,solution2;
    delta = b*b - 4 * a * c;
    if (delta >= 0) {
        solution1 = (-b+sqrt(delta))/(2*a);
        solution2 = (-b-sqrt(delta))/(2*a);
            printf("%lf %lf", solution1, solution2);

    }
    else {
        printf("No Real solutions");
    }
    return 0;

}