
#include<stdio.h>
int main() {
	int x, y;
	scanf("%d %d", &x, &y);
	if (x != y) {
		if (x > y) {
			printf("%d is the largest", x);
		}
		else printf("%d is the largest", y);
	}
	else printf("These numbers are equal");
	return 0;
}