#include <stdio.h>
int main()
{
    int profit, bonus;
    scanf("%d", &profit);
    if (profit <= 100000)
    {
        bonus = 0.1 * profit;
        printf("%d", bonus);
    }
    else
    {
        if (profit <= 200000)
        {
            bonus = 0.1 * 100000 + 0.075 * (profit - 100000);
            printf("%d", bonus);
        }
        else
        {
            if (profit <= 400000)
            {
                bonus = 0.1 * 100000 + 0.075 * 100000 + 0.05 * (profit - 200000);
                printf("%d", bonus);
            }
            else
            {
                if (profit <= 600000)
                {
                    bonus = 0.1 * 100000 + 0.075 * 100000 + 0.05 * 200000 + 0.03 * (profit - 400000);
                    printf("%d", bonus);
                }
                else
                {
                    if (profit <= 1000000)
                    {
                        bonus = 0.1 * 100000 + 0.075 * 100000 + 0.05 * 200000 + 0.03 * 200000 + 0.015 * (profit - 600000);
                        printf("%d", bonus);
                    }
                    else
                    {
                        bonus = 0.1 * 100000 + 0.075 * 100000 + 0.05 * 200000 + 0.03 * 200000 + 0.015 * 400000 + 0.01 * (profit - 1000000);
                        printf("%d", bonus);
                    }
                }
            }
        }
    }
    return 0;
}