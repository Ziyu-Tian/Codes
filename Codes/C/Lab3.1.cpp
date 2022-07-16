//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
int main()
{
	float t;
	char s[10];
	scanf("%f %s", &t, &s[0]);
	if (s[0] == 'C')
	{
		t = 1.8 * t + 32;
		printf("%.2f Fahrenheit", t);
	}
	else
	{
		if (s[0] == 'c')
		{
			t = 1.8 * t + 32;
			printf("%.2f Fahrenheit", t);
		}
		else
		{
			t = (t - 32) / 1.8;
			printf("%.2f Celsius", t);
		}
	}
	return 0;
}