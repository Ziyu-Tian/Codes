#include<iostream>
#include<cstring>
#include<cstdlib>
using namespace std;

int main()
{
    unsigned seed;
    int Line;
    seed = time(0);
    srand(seed);
    cout<<"Please input the dimension of the matrix:"<<endl;
    cin>>Line;
    double a[Line][Line];
    cout<<"Initializing the matrix..."<<endl;
    for (int i = 0; i < Line; i++)
    {
        for (int j = 0; j < Line; j++)
        {
            a[i][j] = rand()%10;
        }
    }
    for (int i = 0; i < Line; i++)
    {
        for (int j = 0; j < Line; j++)
        {
            printf("%.2f ",a[i][j]);
        }
        printf("\n");
        
    }
    
    return 0;

}

double det (int a[][N],int n,int d) // n is the order of matrix, d is the line/column
// to be deleted
{
    if (n==1)
    {
        return a[0][0]; // the exit of the recursive
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            int New[N][N];
            
        }
        
        


    }
    
    
}