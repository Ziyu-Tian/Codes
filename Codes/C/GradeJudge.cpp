#include <stdio.h>
int main()
{
    int grade;
    scanf("%d", &grade);
    if (grade >= 90)
    {
        printf("%c", 'A');
    }
    else
    {
        if (grade >= 80)
        {
            printf("%c", 'B');
        }
        else
        {
            if (grade >= 70)
            {
                printf("%c", 'C');
            }
            else
            {
                printf("%c", 'D');
            }
        }
    }

    return 0;
}
