//#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	int a, b, c;
	scanf("%d %d %d", &a, &b, &c);
	if (a < b) {
		if (b < c) {
			printf("True");
		}	
		else
		printf("False");
	}
	else {
		printf("False");
	}
	return 0;
	
}