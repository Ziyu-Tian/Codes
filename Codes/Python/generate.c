#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
struct Pair // (s,T) pairs
{
    int s;
    int t;
};

int main()
{

    int threshold = 5;
    int sensitivity = 5;

    struct Pair pairs[100];

    for (int i = 0; i < 100; i++)
    {
        if ((i + 1) % 10 == 0)
        {
            pairs[i].s = sensitivity;
            sensitivity += 5;
            pairs[i].t = threshold;
            threshold = 5;
        }
        else
        {
            pairs[i].s = sensitivity;

            pairs[i].t = threshold;
            threshold += 5;
        }
    }

    for (int i = 0; i < 100; i++)
    {
        int a, b;
        a = pairs[i].s;
        b = pairs[i].t;

        printf("'(%d,%d)', ",a,b);
    }
    
    return 0;


}