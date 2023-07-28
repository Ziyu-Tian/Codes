#include <stdio.h>

struct Pair
{
    int s; // sensitivity
    int t; // Threshold
};

int main()
{
    struct Pair pairs[100];
    int sensitivity = 5;
    int threshold = 5;
    for (int i = 0; i < 100; i++)
    {
        
        if ((i+1)%10==0)
        {
            pairs[i].s = sensitivity;
            sensitivity+=5;
            pairs[i].t = threshold; 
            threshold=5;

        }
        else
        {
            pairs[i].s = sensitivity;
           
            pairs[i].t = threshold; 
            threshold+=5;
        }
        
        
       
    }
    for (int i = 0; i < 100; i++)
    {
        printf("Pair %d: s = %d t = %d\n", i + 1, pairs[i].s, pairs[i].t);

    }
}
