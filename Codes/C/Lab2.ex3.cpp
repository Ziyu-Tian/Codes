
#include<stdio.h>
int main() {
	int a, b,c;
	scanf("%d %d %d", &a, &b, &c);
	if ((a == b)&& (b==c))
	{
		printf("They are equal");
	}
	else {
		printf("They are not equal");
	}
	return 0;
}