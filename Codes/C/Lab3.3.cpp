
#include <stdio.h>
int main()
{
	int a, b, c;
	scanf("%d %d %d", &a, &b, &c);
	if (a + b > c)
	{
		printf("It's a triangle ");
		if (a == b || a == c || b == c)
		{
			if (a == b && a == c && b == c)
			{
				printf("and Equilateral");
			}
			else
			{
				printf("and Isosceles");
			}
		}
		else
		{
			printf("and Scalene");
		}
	}
	else
	{
		printf("It's not a triangle");
	}
	return 0;
}