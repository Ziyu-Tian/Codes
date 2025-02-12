#include <stdio.h>
#include <ctype.h>
int main()
{
    char a;
    scanf("%c", &a);
    if (isdigit(a))
    {
        printf("It is a number.");
    }
    else
    {
        if (isalpha(a))
        {
            printf("This an alpha.");
        }
        else
            printf("This is a special character. ");
    }
    return 0;
}