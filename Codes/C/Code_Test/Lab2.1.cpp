//#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	int number, first, second;
	scanf("%d", &number);
	first = number / 10;
	second = number % 10;
	printf("%d %d", first, second);
	return 0;
}