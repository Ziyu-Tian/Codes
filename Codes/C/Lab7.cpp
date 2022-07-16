#include <stdio.h>
#include <string.h>
#include <ctype.h>
int main()
{
    FILE *f = fopen("modules.txt", "r");
    if (f == NULL)
    {
        printf("Error: cannot open file\n");
        return 1;
    }
    char source[200];
    char input[200];
    printf(" Enter a search string: ");
    scanf("%s", input);
    int n = strlen(input);
    int count = 0;
    while (fgets(source, 200, f))
    {
        int match = 1;
        for (int i = 0; i < n; i++)
        {
            if (source[i] != input[i])
            {
                match = 0;
            }
        }
        if (match)
        {
            count = count + 1;
            printf("%s", source);
        }
        
    }

    fclose(f);
    printf("%d", count);
    return 0;
}

