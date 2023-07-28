#include <stdio.h>

struct Pair
{
    int s; // sensitivity
    int t; // Threshold
};

int main()
{
    struct Pair pairs[25];
    int sensitivity = 1;
    int threshold = 1;
    for (int i = 0; i < 25; i++)
    {
        
        if ((i+1)%5==0)
        {
            pairs[i].s = sensitivity;
            sensitivity++;
            pairs[i].t = threshold; 
            threshold = 1;

        }
        else
        {
            pairs[i].s = sensitivity;
           
            pairs[i].t = threshold; 
            threshold++;
        }
        
        
       
    }
    for (int i = 0; i < 25; i++)
    {
        printf("Pair %d: s = %d t = %d\n", i + 1, pairs[i].s, pairs[i].t);

    }
}
