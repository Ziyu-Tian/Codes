#include <iostream>
#include <algorithm>
using namespace std;
int lcd(int a[], int n);
int main()
{
    int n;
    int a[10];
    cout << "Please input the number of the input:" << endl;
    cin >> n;
    cout << "Please input the numbers:" << endl;
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
        // input the number list
    }
    cout << lcd(a, n);
    return 0;
}
int lcd(int a[], int n)
{
    int max = *max_element(a, a + n); // max_element function can return the address
    // of the max elements of the array
    int sum = 0;
    for (;; max++)
    {   
        // The LCD must larger or equal to the max of array, adding from the max 
        // reduce the times of searching 
        for (int j = 0; j < n; j++)
        {
            if (max % a[j] == 0)
            {
                sum++;
                // sum is the number of array members which is suitable for the max
            }
            else
            {
                sum = 0;
                break;
                // As long as one of the member is not suitable, break and try others
            }
        }
        if (sum == n)
        {
            break;
            // sum == n means it is the final answer
        }
    }
    return max;
}