#include <stdio.h>
#include <string.h>
int stringlength(const char str[]);
int stringlength(const char str[])
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
        ;
    return i;
}
int main()
{
    char a[1000];
    printf("Input the string:");
    gets(a);
    printf("The length of the string is: %d", stringlength(a));
    return 0;
}