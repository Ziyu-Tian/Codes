#include <stdio.h>
#include <ctype.h>
#include <string.h>
int main()
{
    char a[200];
    int alpha = 0;
    int digit = 0;
    int space = 0;
    int other = 0;
    fgets(a,sizeof(a),stdin);
    for (unsigned int i = 0; i < strlen(a); i++)
    {
        if (isdigit(a[i]))
        {
            digit += 1;
        }
        else
        {
            if (isalpha(a[i]))
            {
                alpha += 1;
            }
            else
            {
                if (isspace(a[i]))
                {
                    space += 1;
                }
                else
                {
                    other += 1;
                }
            }
        }
    }
    printf("%d %d %d %d", alpha, digit, space-1, other);
    return 0;
}