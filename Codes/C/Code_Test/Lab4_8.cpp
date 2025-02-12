#include <stdio.h>
#include <string.h>
int main()
{
    char str[1000];
    char num[1000] = {"0123456789"};
    int frequency = 0;
    printf("Please input any number: ");
    fgets(str, sizeof(str), stdin);
    for (int i = 0; num[i] != '\0'; i++)
    {
        for (int j = 0; str[j] != '\0'; j++)
        {
            if (num[i] == str[j])
            {
                ++frequency;
            }
        }
        printf("Frequency of %c = %d\n", num[i], frequency);
        frequency=0;
    }
    return 0;
}