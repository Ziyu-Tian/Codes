//#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	float x, y;
	float average;
	scanf("%f %f", &x, &y);
	average = (x + y) / 2;
	printf("%.1f", average);
	return 0;
}