#include<iostream>
#include<algorithm>
using namespace std;
int lcd(int a[],int n);
int main()
{
    int n;
    int a[10];
    cout<<"Please input the number of the input:"<<endl;
    cin>>n;
    cout<<"Please input the numbers:"<<endl;
    for (int i = 0; i < n; i++)
    {
        cin>>a[i];
    }
    cout<<lcd(a,n);
    return 0;

}
int lcd(int a[],int n)
{
    int max = *max_element(a,a+n);
    int sum=0;
    for ( ;;max++)
    {
        for (int j = 0;j<n; j++)
        {
            if (max%a[j]==0)
            {
                sum++;
            }
            else
            {
             sum = 0;
             break;   
            }
            
        }
        if (sum==n)
        {
            break;
        } 
    }
    return max;
    
}