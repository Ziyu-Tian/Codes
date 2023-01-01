#include <bits/stdc++.h>
using namespace std;
int main()
{
    string a;
    cout << "Please input the sequence: "<< endl;
    cin >> a;
    sort(a.begin(), a.end());// qsort the string, make the value min before loop
    cout << "The sequence have these permutations: "<< endl;
    do // do while make loop first then judge the range
    {
        cout << a << endl;
    } while (next_permutation(a.begin(), a.end()));
    // next_permutation(begin,end) can make 'a' to the next larger composition
    
    
    return 0;
}