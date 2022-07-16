//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main()
{
	float t;
	char s;
	scanf("%f %c", &t, &s);
	switch (s)
	{
	case 'C':
		t = t * 1.8 + 32;
		printf("%.2f F", t);
		break;
	case 'F':
		t = (t - 32) / 1.8;
		printf("%.2f C", t);
		break;
	default:
		printf("Illegal Input!");
	}
	return 0;
}