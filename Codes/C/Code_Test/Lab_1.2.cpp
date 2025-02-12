//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main() {
	int x, y;
	int product, difference, quotient, remainder;
	scanf("%d", &x);
	scanf("%d", &y);
	product = x + y;
	difference = x - y;
	quotient = x / y;
	remainder = x % y;
	printf("%d ", product);
	printf("%d ", difference);
	printf("%d ", quotient);
	printf("%d", remainder);
	return 0;

}