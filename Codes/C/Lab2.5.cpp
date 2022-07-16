//#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	int a;
	scanf("%d", &a);
	(a % 2 == 0) ? printf("Even") : printf("Odd");
	return 0;
}