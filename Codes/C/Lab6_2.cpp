/*
#include <stdio.h>
#include <string.h>
void sort(char str[]);
int stringlength(const char str[]);
void sort(char str[])
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        char temp;
        for (int j = i + 1; str[j] != '\0'; j++)
        {
            if (str[j] < str[i])
            {
                temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}
int stringlength(const char str[])
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
        ;
    return i;
}
int main()
{
    char s[1000];
    printf("Input the string: ");
    gets(s);
    sort(s);
    for (int i = 0; i < stringlength(s); i++)
    {
        printf("%c ",s[i]);
    }
    return 0;
    
}
*/
#include <stdio.h>
#define SIZE 20

size_t getSize(float *ptr); // prototype

int main(void)
{
   float array[SIZE]; // create array

   printf("The number of bytes in the array is %lu"            
          "\nThe number of bytes returned by getSize is %lu\n",
          sizeof(array), getSize(array));                
} 

// return size of ptr
size_t getSize(float *ptr)
{
   return sizeof(ptr);
}

